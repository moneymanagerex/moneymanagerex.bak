//----------------------------------------------------------------------------
#include <limits>
#include <algorithm> // min, max
#include <math.h>
//----------------------------------------------------------------------------
#include "chart.h"
//----------------------------------------------------------------------------

namespace
{

int ROUND( double x )
{
        double res = floor( x + 0.5 );
        return static_cast<int>( res ); // (int)(x + 0.5)
}
//----------------------------------------------------------------------------

float getFrequency(float vmax)
{
    const int max_steps = 5;
    float step = vmax/max_steps;

    const float base = 10;
    int y = 0;

    while ( pow(base, ++y) < step );
    float new_step = pow(base, --y);

    if (step > new_step)
    {
        size_t cnt = floor(step/new_step + 0.5f);
        if (cnt > 1)
            new_step *= cnt;
    }

    return new_step;
}
//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

ChartData::ChartData( const wxString &aKey, float aVal ) :
                key( aKey ),
                val( aVal ),
                aval( aVal )
{
}
//----------------------------------------------------------------------------

ChartData::ChartData( const wxString &aKey, const std::vector<float> &aSerie ) :
                key( aKey ),
                val(),
                aval(),
                serie( aSerie )
{
}
//----------------------------------------------------------------------------

AbstractChart::AbstractChart( int aWidth, int aHeight ) :
                width( aWidth ),
                height( aHeight )
{
    int i = -1;
    palete[++i] = wxColour( 0x00, 0x79, 0xEA );
    palete[++i] = wxColour( 0xee, 0x2A, 0x00 );
    palete[++i] = wxColour( 0xF7, 0x97, 0x31 );
    palete[++i] = wxColour( 0xBD, 0x7F, 0xAE );
    palete[++i] = wxColour( 0xFF, 0xF3, 0xAB );
    palete[++i] = wxColour( 0x66, 0xAE, 0x3F );
    palete[++i] = wxColour( 0xBB, 0x7F, 0xB8 );
    palete[++i] = wxColour( 0x64, 0x91, 0xAA );
    palete[++i] = wxColour( 0xE8, 0xC1, 0x45 );
    palete[++i] = wxColour( 0x2B, 0x96, 0xE7 );
    palete[++i] = wxColour( 0xD2, 0x9A, 0xf7 );
    palete[++i] = wxColour( 0x8F, 0xEA, 0x7B );
    palete[++i] = wxColour( 0xFF, 0xFF, 0x3B );
    palete[++i] = wxColour( 0x58, 0xCC, 0xCC );
    palete[++i] = wxColour( 0x7A, 0xB3, 0x3E );
    palete[++i] = wxColour( 0x42, 0x44, 0x3F );
    palete[++i] = wxColour( 0xFC, 0xAC, 0x00 );
    palete[++i] = wxColour( 0xA2, 0xE1, 0x4A );
    palete[++i] = wxColour( 0xa8, 0x62, 0x16 );
    palete[++i] = wxColour( 0xC3, 0xD9, 0xFF );
    palete[++i] = wxColour( 0xC7, 0x98, 0x10 );
    palete[++i] = wxColour( 0x6B, 0xBA, 0x70 );
    palete[++i] = wxColour( 0xCD, 0xEB, 0x8B );
    palete[++i] = wxColour( 0xD0, 0x1F, 0x3C );

    wxASSERT( ++i == PAL_MAX );

    image = wxBitmap( width, height );
    dc.SelectObject( image );

    ClearFillColour();
    ClearStrokeColour();

    plainFont = wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    boldFont = wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
}
//----------------------------------------------------------------------------

void AbstractChart::SetBackground( const wxColour& colour )
{
    ClearStrokeColour();
    SetFillColour( colour );
    dc.Clear();
}
//----------------------------------------------------------------------------

void AbstractChart::SetFillColour( const wxColour& colour, int style )
{
    wxBrush brush = wxBrush( colour, style );
    dc.SetBrush( brush );
}
//----------------------------------------------------------------------------

void AbstractChart::SetStrokeColour( const wxColour& colour, int width, int style )
{
    wxPen pen = wxPen( colour, width, style );
    dc.SetPen( pen );
}
//----------------------------------------------------------------------------

void AbstractChart::ClearFillColour()
{
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
}
//----------------------------------------------------------------------------

void AbstractChart::ClearStrokeColour()
{
    dc.SetPen( *wxTRANSPARENT_PEN );
}
//----------------------------------------------------------------------------

bool AbstractChart::Save( const wxString& file )
{
    wxImage pic = image.ConvertToImage();
    //make it transparent
    pic.SetMaskColour(255,255,255);
    return pic.SaveFile( file, wxBITMAP_TYPE_PNG );
}
//----------------------------------------------------------------------------

void AbstractChart::SetData( const std::vector<ChartData> &aData, bool simpleValue )
{
    data = aData;

    if ( !simpleValue ) return;

    float vtotal = 0;

    for ( size_t i = 0; i < data.size(); ++i )
        vtotal += fabs( data[i].val );

    // normalize 0..100 for percentage charts
    for ( size_t i = 0; i < data.size(); ++i )
        data[i].val = fabs( data[i].val ) * 100.0 / vtotal;
}
//----------------------------------------------------------------------------

void AbstractChart::SetData( const std::vector<ChartData> &aData, const std::vector<wxString> &aSerieLabel )
{
    SetData( aData, false );
    serieLabel = aSerieLabel;
}
//----------------------------------------------------------------------------

PieChart::PieChart( int aWidth, int aHeight ) : AbstractChart( aWidth, aHeight )
{
}
//----------------------------------------------------------------------------

void PieChart::Init( int aChartsize, EChartLegend aLegendMode, EPieChartStyle aPieMode )
{
    csize = aChartsize;
    keymode = aLegendMode;
    mode = aPieMode;

    SetBackground( *wxWHITE );
}
//----------------------------------------------------------------------------

bool PieChart::Render( const wxString& title )
{
    if ( data.empty() ) return false;

    if ( !title.empty() )
    {
        SetStrokeColour( *wxBLACK );
        ClearFillColour();

        dc.SetFont( boldFont );
        dc.DrawText( title, 10, 20 - dc.GetTextExtent( title ).GetHeight() );
    }

    // where to start drawing from
    int originLeft = 0;

    if ( keymode == CHART_LEGEND_FIXED )
        originLeft = 10;
    else
        originLeft = ( width - csize ) / 2;

    ClearStrokeColour();
    SetFillColour( *wxBLACK );

    // background
    dc.DrawEllipse( originLeft - 2, ROUND( ( ( height - 12 ) / 2 ) - csize / 2.0 ) + 12, csize + 4, csize + 4 );

    if ( keymode == CHART_LEGEND_FIXED )
    {
        SetStrokeColour( *wxBLACK );
        ClearFillColour();

        dc.SetFont( boldFont );
        wxString legend = _( "Legend:" );
        dc.DrawText( legend, csize + 23, 25 - dc.GetTextExtent( legend ).GetHeight() );

        dc.SetFont( plainFont );

        for ( size_t i = 0; i < data.size(); ++i )
        {
            if ( mode == PIE_CHART_PERCENT )
                dc.DrawText(
                        data[i].key + wxT( " (" ) + wxString::Format( wxT( "%.2f" ), data[i].val ) + wxT( "%)" ),
                        csize + 37,
                        40 + i * 18 - 11 );
            else
                dc.DrawText(
                        data[i].key, // + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")")
                        csize + 37,
                        40 + i * 18 - 11 );
        }

        dc.DrawRectangle(
                csize + 18,
                10,
                ( width - ( csize + 18 ) - 10 ),
                ( int ) ( 20 + data.size() * 18 ) );
    }
    else
    {
        int l = originLeft + ROUND( csize / 2.0 );
        int i1 = ROUND( ( height - 12 ) / 2 ) + 12;

        float f1 = csize / 2.0 + 6.0;  // offset of the label to the pie
        float f2 = 0.0;

        for ( size_t j = 0; j < data.size(); ++j )
        {
            float f3 = ( f2 + data[j].val / 2.0 ) - 25.0;
            int j1 = l + ( int ) ( f1 * cos( f3 * 0.062831799999999993 ) );
            int k1 = i1 + ( int ) ( f1 * sin( f3 * 0.062831799999999993 ) );

            dc.DrawLine( l, i1, j1, k1 );
            dc.DrawLine( j1, k1, j1 + 6 * ( ( j1 > l ) ? 1 : -1 ), k1 );

            dc.SetFont( plainFont );
            wxString s;

            if ( mode == PIE_CHART_PERCENT )
                s = data[j].key + wxT( " (" ) + wxString::Format( wxT( "%.2f" ), data[j].val ) + wxT( "%)" );
            else
                s = data[j].key; // + wxT(" (") + wxString::Format(wxT("%.2f"), data[j].aval) + wxT(")")

            int l1;

            if ( j1 > l )
                l1 = j1 + 6 + 1;
            else
                l1 = j1 - 6 - ( dc.GetTextExtent( s ).GetWidth() + 1 );

            int i2 = k1 + 6;
            dc.DrawText( s, l1, i2 - dc.GetTextExtent( s ).GetHeight() );
            f2 += data[j].val;
        }
    }

    float f = 25.0;

    for ( size_t k = 0; k < data.size(); ++k )
    {
        ClearStrokeColour();
        SetFillColour( palete[k % PAL_MAX] );

        if ( keymode == CHART_LEGEND_FIXED )
            dc.DrawRectangle( csize + 23, 30 + k * 18, 12, 12 );

        dc.DrawEllipticArc(
                originLeft,
                ROUND( ( ( height - 12 ) / 2 ) - csize / 2.0 ) + 12,
                csize,
                csize,
                ROUND( f * 3.6 ) + ceil( data[k].val * 3.6 ) * -1,
                ROUND( f * 3.6 ) );
        f -= data[k].val;

        if ( f < 0.0 )
            f += 100;
    }

    return true;
}
//----------------------------------------------------------------------------

BarChart::BarChart( int aWidth, int aHeight ) : AbstractChart( aWidth, aHeight )
{
}
//----------------------------------------------------------------------------

void BarChart::Init( int aChartsize, EChartLegend aLegendMode, EBarChartStyle aMode )
{
    csize = aChartsize;
    keymode = aLegendMode;
    mode = aMode;

    SetBackground( *wxWHITE );
}
//----------------------------------------------------------------------------

bool BarChart::Render( const wxString& title )
{
    if ( data.empty() ) return false;

    // calculate min/max value (boundaries)
    float min = 0;
    float max = 0;

    if ( mode == BAR_CHART_SIMPLE )
    {
        // this forces the chart to start from 0 if values are all greater than 0
        min = 0;
        max = data[0].aval;

        for ( size_t i = 0; i < data.size(); ++i )
        {
            min = std::min( min, data[i].aval );
            max = std::max( max, data[i].aval );
        }
    }
    else
    {
        // this forces the chart to start from 0 if values are all greater than 0
        min = 0;
        max = data[0].serie[0];

        for ( size_t i = 0; i < data.size(); ++i )
        {
            for ( size_t j = 0; j < data[i].serie.size(); ++j )
            {
                min = std::min( min, data[i].serie[j] );
                max = std::max( max, data[i].serie[j] );
            }
        }
    }

    // avoid out of scale for empty data sets
    if ( min == max ) max = 1;

    const float freq = getFrequency( fabs(min) + fabs(max) );

    if ( !title.empty() )
    {
        SetStrokeColour( *wxBLACK );
        ClearFillColour();

        dc.SetFont( boldFont );
        dc.DrawText( title, 10, 20 - dc.GetTextExtent( title ).GetHeight() );
    }

    // where to start drawing from
    int originLeft = 1;

    while ( max > pow( 10.0f, originLeft++ ) );

    originLeft *= 7;

    int gap = 5;

    int barwidth;

    if ( mode == BAR_CHART_SIMPLE )
    {
        if ( keymode == CHART_LEGEND_FIXED )
        {
            barwidth = ( int ) ( ( csize - gap * data.size() ) / data.size() );
        }
        else
        {
            barwidth = ( int ) ( ( width - originLeft - gap * data.size() - 10 ) / data.size() );
        }
    }
    else
    {
        if ( keymode == CHART_LEGEND_FIXED )
        {
            barwidth = ( int ) ( ( csize - gap * data.size() ) / ( data.size() * serieLabel.size() ) );
        }
        else
        {
            barwidth = ( int ) ( ( width - originLeft - gap * data.size() - 10 ) / ( data.size() * serieLabel.size() ) );
        }
    }

    // draw the legend/key of the chart
    if ( keymode == CHART_LEGEND_FIXED )
    {
        SetStrokeColour( *wxBLACK );
        ClearFillColour();

        dc.SetFont( boldFont );
        wxString legend = _( "Legend:" );
        dc.DrawText( legend, originLeft + csize + 15, 25 - dc.GetTextExtent( legend ).GetHeight() );

        dc.SetFont( plainFont );

        if ( mode == BAR_CHART_SIMPLE )
        {
            for ( size_t i = 0; i < data.size(); ++i )
            {
                dc.DrawText(
                        data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */,
                        originLeft + csize + 30,
                        43 + i * 18 - dc.GetTextExtent( legend ).GetHeight() );
            }
        }
        else
        {
            for ( size_t i = 0; i < serieLabel.size(); ++i )
            {
                dc.DrawText(
                        serieLabel[i] /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */,
                        originLeft + csize + 30,
                        43 + i * 18 - dc.GetTextExtent( legend ).GetHeight() );
            }
        }

        if ( mode == BAR_CHART_SIMPLE )
        {
            dc.DrawRectangle(
                    originLeft + csize + 10,
                    10,
                    width - ( originLeft + csize + 15 ),
                    ( int ) ( 20 + data.size() * 18 ) );
        }
        else
        {
            dc.DrawRectangle(
                    originLeft + csize + 10,
                    10,
                    width - ( originLeft + csize + 15 ),
                    ( int ) ( 20 + serieLabel.size() * 18 ) );
        }

        // draw axis
        dc.DrawLine( originLeft - 3, height - 25, originLeft + csize + 3, height - 25 );
        dc.DrawLine( originLeft, height - 22, originLeft, 22 );
        // vertical axis labels
        int y = height - 25;
        int step = static_cast<int>( ( height - 50 ) / ( ( fabs( min ) + fabs( max ) ) / freq ) );

        for ( float fi = min; fi <= max; fi += freq )
        {
            // draw the label
            ClearFillColour();
            SetStrokeColour( *wxBLACK );

            wxString label = wxString::Format( wxT( "%.0f" ), fi );
            dc.DrawText( label, 3, y - dc.GetTextExtent( label ).GetHeight() );

            // 1st entry skip grid, otherwise axis is overriden
            if ( fi != min )
            {
                // draw grid
                ClearFillColour();
                SetStrokeColour( *wxLIGHT_GREY );

                dc.DrawLine( originLeft + 3, y, originLeft + csize - 3, y );
            }

            y -= step;
        }


        if ( mode == BAR_CHART_SERIES )
        {
            int labelOffset = ( int ) ( ( ( barwidth * serieLabel.size() ) + gap ) / 2 );

            SetStrokeColour( *wxBLACK );
            ClearFillColour();

            for ( int i = 0; i < static_cast<int>( data.size() ); ++i )
            {
                wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                wxCoord x = originLeft + i * ( barwidth * static_cast<int>( serieLabel.size() ) ) + ( i + 1 ) * gap + labelOffset - ( dc.GetTextExtent( key ).GetWidth() / 2 );
                wxCoord y = height - 10 - dc.GetTextExtent( key ).GetHeight();
                dc.DrawText( key, x, y );
            }
        }
    }
    else
    {
        SetStrokeColour( *wxBLACK );
        ClearFillColour();

        // draw axis
        dc.DrawLine( originLeft - 3, height - 25, width - 5, height - 25 );
        dc.DrawLine( originLeft, height - 22, originLeft, 22 );

        dc.SetFont( plainFont );

        int y = height - 25;
        int step = static_cast<int>( ( height - 50 ) / ( ( fabs( min ) + fabs( max ) ) / freq ) );

        // horiz axis labels
        if ( mode == BAR_CHART_SIMPLE )
        {
            int labelOffset = ( barwidth + gap ) / 2;
            labelOffset -= 20;

            for ( size_t i = 0; i < data.size(); ++i )
            {
                wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                dc.DrawText(
                        key,
                        originLeft + i*barwidth + ( i + 1 )*gap + labelOffset /* - (dc.GetTextExtent(key).GetLength() / 2) */,
                        height - 10 - dc.GetTextExtent( key ).GetHeight() );
            }
        }

        // vertical axis labels
        for ( float fi = min; fi <= max; fi += freq )
        {
            // draw the label
            ClearFillColour();
            SetStrokeColour( *wxBLACK );

            wxString label = wxString::Format( wxT( "%.0f" ), fi );
            dc.DrawText( label, 3, y - dc.GetTextExtent( label ).GetHeight() );

            // 1st entry skip grid, otherwise axis is overriden
            if ( fi != min )
            {
                    // draw grid
                    ClearFillColour();
                    SetStrokeColour( *wxLIGHT_GREY );

                    dc.DrawLine( originLeft + 3, y, width - 8, y );
            }

            y -= step;
        }

        if ( mode == BAR_CHART_SERIES )
        {
            int labelOffset = ( int ) ( ( ( barwidth * serieLabel.size() ) + gap ) / 2 );

            SetStrokeColour( *wxBLACK );
            ClearFillColour();

            for ( int i = 0; i < static_cast<int>( data.size() ); ++i )
            {
                wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                wxCoord x = originLeft + i * ( barwidth * static_cast<int>( serieLabel.size() ) ) + ( i + 1 ) * gap + labelOffset /* - (dc.GetTextExtent(key).GetLength() / 2) */;
                wxCoord y = height - 10 - dc.GetTextExtent( key ).GetHeight();
                dc.DrawText( key, x, y );
            }
        }
    }

    // draw the bars
    if ( mode == BAR_CHART_SIMPLE )
    {
        int zero = ( int ) ( min * ( height - 50 ) / ( max - min ) );

        for ( size_t k = 0; k < data.size(); ++k )
        {
            ClearStrokeColour();
            SetFillColour( palete[k % PAL_MAX] );

            int h = static_cast<int>( data[k].aval * ( height - 50 ) / ( max - min ) );

            if ( h == 0 )
            {
                if ( data[k].aval > 0 )
                {
                    h = 1;
                }
                else if ( data[k].aval < 0 )
                {
                    h = -1;
                }
            }

            if ( data[k].aval >= 0 )
            {
                dc.DrawRectangle(
                        originLeft + k * barwidth + ( k + 1 ) * gap,
                        ( height - 25 ) + zero - h,
                        barwidth,
                        h );
            }
            else
            {
                dc.DrawRectangle(
                        originLeft + k * barwidth + ( k + 1 ) * gap,
                        ( height - 25 ) + zero,
                        barwidth,
                        -h );
            }

            // key tag
            if ( keymode == CHART_LEGEND_FIXED )
            {
                dc.DrawRectangle(
                        originLeft + csize + 15,
                        33 + k * 18,
                        10,
                        10 );
            }
        }
    }
    else
    {
        int zero = ( int ) ( min * ( height - 50 ) / ( max - min ) );

        for ( size_t k = 0; k < data.size(); ++k )
        {
            for ( size_t j = 0; j < serieLabel.size(); ++j )
            {
                ClearStrokeColour();
                SetFillColour( palete[j % PAL_MAX] );

                int h = ( int ) ( data[k].serie[j] * ( height - 50 ) / ( max - min ) );

                if ( h == 0 )
                {
                    if ( data[k].aval > 0 )
                    {
                        h = 1;
                    }
                    else if ( data[k].aval < 0 )
                    {
                        h = -1;
                    }
                }

                if ( data[k].serie[j] >= 0 )
                {
                    wxCoord x = originLeft + ( k * static_cast<int>( serieLabel.size() ) + j ) * barwidth + ( k + 1 ) * gap;
                    wxCoord y = ( height - 25 ) + zero - h;
                    dc.DrawRectangle( x, y, barwidth, h );
                }
                else
                {
                    wxCoord x = originLeft + ( k * static_cast<int>( serieLabel.size() ) + j ) * barwidth + ( k + 1 ) * gap;
                    wxCoord y = ( height - 25 ) + zero;
                    dc.DrawRectangle( x, y, barwidth, -h );
                }

                // key tag
                if ( keymode == CHART_LEGEND_FIXED )
                {
                    // only once
                    if ( k == 0 )
                    {
                        dc.DrawRectangle(
                                originLeft + csize + 15,
                                33 + j * 18,
                                10,
                                10 );
                    }
                }
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
