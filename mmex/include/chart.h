#ifndef _MM_EX_CHART_H_
#define _MM_EX_CHART_H_

#include "defs.h"

#include <wx/dcmemory.h>
#include <wx/bitmap.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/colour.h>

#include <limits>
#include <algorithm> // min, max
#include <math.h>


inline int ROUND(double x)
{
    double res = floor(x + 0.5);
    return static_cast<int>(res); // (int)(x + 0.5)
}


class ChartData
{
public:
	ChartData(wxString aKey, float aVal = 0.0) : val(aVal), aval(aVal), key(aKey)
	{
	};

	ChartData(wxString aKey, std::vector<float> aSerie) : key(aKey), serie(aSerie)
	{
	};
	
	wxString key;
	
	// normalized val
	float val;
	// real val
	float aval;
	
	std::vector<float> serie;
};

enum { CHART_LEGEND_FIXED, CHART_LEGEND_FLOAT };

class AbstractChart
{
public:
	AbstractChart(int aWidth, int aHeight) : width(aWidth), height(aHeight)
	{
        int i = -1;
        palete[++i] = wxColour(0x00, 0x79, 0xEA);
        palete[++i] = wxColour(0xee, 0x2A, 0x00);
        palete[++i] = wxColour(0xF7, 0x97, 0x31);
        palete[++i] = wxColour(0xBD, 0x7F, 0xAE);
        palete[++i] = wxColour(0xFF, 0xF3, 0xAB);
        palete[++i] = wxColour(0x66, 0xAE, 0x3F);
        palete[++i] = wxColour(0xBB, 0x7F, 0xB8);
        palete[++i] = wxColour(0x64, 0x91, 0xAA);
        palete[++i] = wxColour(0xE8, 0xC1, 0x45);
        palete[++i] = wxColour(0x2B, 0x96, 0xE7);
        palete[++i] = wxColour(0xD2, 0x9A, 0xf7);
        palete[++i] = wxColour(0x8F, 0xEA, 0x7B);
        palete[++i] = wxColour(0xFF, 0xFF, 0x3B);
        palete[++i] = wxColour(0x58, 0xCC, 0xCC);
        palete[++i] = wxColour(0x7A, 0xB3, 0x3E);
        palete[++i] = wxColour(0x42, 0x44, 0x3F);
        palete[++i] = wxColour(0xFC, 0xAC, 0x00);
        palete[++i] = wxColour(0xA2, 0xE1, 0x4A);
        palete[++i] = wxColour(0xa8, 0x62, 0x16);
        palete[++i] = wxColour(0xC3, 0xD9, 0xFF);
        palete[++i] = wxColour(0xC7, 0x98, 0x10);
        palete[++i] = wxColour(0x6B, 0xBA, 0x70);
        palete[++i] = wxColour(0xCD, 0xEB, 0x8B);
        palete[++i] = wxColour(0xD0, 0x1F, 0x3C);
        
        wxASSERT(++i == PAL_MAX);

		image = wxBitmap(width, height);
		dc.SelectObject(image);

		ClearFillColour();
		ClearStrokeColour();
		
		plainFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		boldFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	};

	void SetBackground(const wxColour& colour)
	{
		ClearStrokeColour();
		SetFillColour(colour);
		dc.Clear();
	};

	void SetFillColour(const wxColour& colour, int style = wxSOLID)
	{
		wxBrush brush = wxBrush(colour, style);
		dc.SetBrush(brush);
	};

	void SetStrokeColour(const wxColour& colour, int width = 1, int style = wxSOLID)
	{
		wxPen pen = wxPen(colour, width, style);
		dc.SetPen(pen);
	};

	void ClearFillColour()
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
	};

	void ClearStrokeColour()
	{
		dc.SetPen(*wxTRANSPARENT_PEN);
	};

	bool Save(const wxString& file)
	{
		return image.SaveFile(file, wxBITMAP_TYPE_PNG);
	};

	void SetData(std::vector<ChartData> aData, bool simpleValue = true)
	{
		data = aData;
		if(simpleValue)
		{
			float vtotal = 0.0;

			for (unsigned int i = 0; i < data.size(); i++)
			{
				vtotal += fabs(data[i].val);
			}
			// normalize 0..100 for percentage charts
			for (unsigned int i = 0; i < data.size(); i++)
			{
				data[i].val = fabs(data[i].val) * 100.0 / vtotal;
			}
		}
	};
	
	void SetData(std::vector<ChartData> aData, std::vector<wxString> aSerieLabel)
	{
		SetData(aData, false);
		serieLabel = aSerieLabel;
	};
	
	virtual bool Render(const wxString& title) = 0;

protected:

	int GetFrequency(int j)
	{
		int k = 1;
		if (j > 10 && j < 21)
			k = 5;
		if (j > 20 && j < 101)
			k = 10;
		if (j > 100 && j < 201)
			k = 50;
		if (j > 200 && j < 1001)
			k = 100;
		if (j > 1000 && j < 2001)
			k = 500;
		if (j > 2000 && j < 10001)
			k = 1000;
		if (j > 10000 && j < 20001)
			k = 5000;
		if (j > 20000 && j < 0x186a1)
			k = 10000;
		if (j > 100000 && j < 1000001)
			k = 100000;
		if (j > 1000000 && j < 10000001)
			k = 1000000;
		if (j > 10000000 && j < 100000001)
			k = 10000000;
		if (j > 100000000)
			k = 100000000;
		return k;
	};

protected:
	std::vector<ChartData> data;
	std::vector<wxString> serieLabel;
	
    enum { PAL_MAX = 24 };
    wxColour palete[PAL_MAX];
	
	wxMemoryDC dc;
	int width;
	int height;

	wxFont plainFont;
	wxFont boldFont;
	
private:
	wxBitmap image;
};

enum { PIE_CHART_PERCENT, PIE_CHART_ABSOLUT };

class PieChart : public AbstractChart
{
public:
	PieChart(int aWidth, int aHeight) : AbstractChart(aWidth, aHeight)
	{
	};
	
	void Init(int aChartsize, int aLegendMode = CHART_LEGEND_FIXED, int aPieMode = PIE_CHART_PERCENT)
	{
		csize = aChartsize;
		keymode = aLegendMode;
		mode = aPieMode;
		
		SetBackground(*wxWHITE);
	};

	bool Render(const wxString& title = wxT(""))
	{
		// no data no chart...
		if(data.size() == 0) return false;
		
		if(title != wxT(""))
		{
			SetStrokeColour(*wxBLACK);
			ClearFillColour();

			dc.SetFont(boldFont);
			dc.DrawText(title, 10, 20 - dc.GetTextExtent(title).GetHeight());
		}

		// where to start drawing from
		int originLeft;

		if (keymode == CHART_LEGEND_FIXED)
			originLeft = 10;
		else
			originLeft = (width - csize) / 2;

		ClearStrokeColour();
		SetFillColour(*wxBLACK);
		
		// background
		dc.DrawEllipse(originLeft - 2, ROUND(((height - 12) / 2) - csize / 2.0) + 12, csize + 4, csize + 4);

		if (keymode == CHART_LEGEND_FIXED)
		{
			SetStrokeColour(*wxBLACK);
			ClearFillColour();

			dc.SetFont(boldFont);
			wxString legend = _("Legend:");
			dc.DrawText(legend, csize + 23, 25 - dc.GetTextExtent(legend).GetHeight());
			
			dc.SetFont(plainFont);
			
			for (unsigned int i = 0; i < data.size(); i++) {
				if(mode == PIE_CHART_PERCENT)
					dc.DrawText(
							data[i].key + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].val) + wxT("%)"),
							csize + 37,
							40 + i * 18 - 11);
				else
					dc.DrawText(
							data[i].key, // + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")")
							csize + 37,
							40 + i * 18 - 11);
			}

			dc.DrawRectangle(
					csize + 18,
					10,
					(width - (csize + 18) - 10),
					(int) (20 + data.size() * 18));
		}
		else
		{
			int l = originLeft + ROUND(csize / 2.0);
			int i1 = ROUND((height - 12) / 2) + 12;
			
			float f1 = csize / 2.0 + 6.0;  // offset of the label to the pie
			float f2 = 0.0;
			
			for (unsigned int j = 0; j < data.size(); j++)
			{
				float f3 = (f2 + data[j].val / 2.0) - 25.0;
				int j1 = l + (int) (f1 * cos(f3 * 0.062831799999999993));
				int k1 = i1 + (int) (f1 * sin(f3 * 0.062831799999999993));
				
				dc.DrawLine(l, i1, j1, k1);
				dc.DrawLine(j1, k1, j1 + 6 * ((j1 > l)?1:-1), k1);
				
				dc.SetFont(plainFont);
				wxString s;

				if (mode == PIE_CHART_PERCENT)
					s = data[j].key + wxT(" (") + wxString::Format(wxT("%.2f"), data[j].val) + wxT("%)");
				else
					s = data[j].key; // + wxT(" (") + wxString::Format(wxT("%.2f"), data[j].aval) + wxT(")")

				int l1;

				if (j1 > l)
					l1 = j1 + 6 + 1;
				else
					l1 = j1 - 6 - (dc.GetTextExtent(s).GetWidth() + 1);

				int i2 = k1 + 6;
				dc.DrawText(s, l1, i2 - dc.GetTextExtent(s).GetHeight());
				f2 += data[j].val;
			}
		}
		float f = 25.0;
		for (unsigned int k = 0; k < data.size(); k++)
		{
			ClearStrokeColour();
			SetFillColour(palete[k % PAL_MAX]);

			if (keymode == CHART_LEGEND_FIXED)
				dc.DrawRectangle(csize + 23, 30 + k * 18, 12, 12);

			dc.DrawEllipticArc(
					originLeft,
					ROUND(((height - 12) / 2) - csize / 2.0) + 12,
					csize,
					csize,
					ROUND(f * 3.6) + ceil(data[k].val * 3.6) * -1,
					ROUND(f * 3.6));
			f -= data[k].val;
			if (f < 0.0)
				f += 100;
		}
		
		return true;
	};

private:
	int csize;
	int keymode;
	int mode;
};

enum { BAR_CHART_SIMPLE, BAR_CHART_SERIES };

class BarChart : public AbstractChart
{
public:
	BarChart(int aWidth, int aHeight) : AbstractChart(aWidth, aHeight)
	{
	};
	
	void Init(int aChartsize, int aLegendMode = CHART_LEGEND_FIXED, int aMode = BAR_CHART_SIMPLE)
	{
		csize = aChartsize;
		keymode = aLegendMode;
		mode = aMode;

		SetBackground(*wxWHITE);
	};

	bool Render(const wxString& title = wxT(""))
	{
        // no data no chart...
		if(data.size() == 0) return false;

		// calculate min/max value (boundaries)
		float min = 0;
		float max = 0;

		if(mode == BAR_CHART_SIMPLE)
		{
			// this forces the chart to start from 0 if values are all greater than 0
			min = 0.0f;
			max = data[0].aval;

			for(unsigned int i=0; i<data.size(); i++)
			{
                min = std::min(min, data[i].aval);
				max = std::max(max, data[i].aval);
			}
		}
		else
		{
			// this forces the chart to start from 0 if values are all greater than 0
			min = 0.0f;
			max = data[0].serie[0];

			for(unsigned int i=0; i<data.size(); i++)
			{
				for(unsigned int j=0; j<data[i].serie.size(); j++)
				{
                    min = std::min(min, data[i].serie[j]);
					max = std::max(max, data[i].serie[j]);
				}
			}
		}

        // avoid out of scale for empty data sets
		if(min == max) max = 1.0;

		// get the freq
		int fpar = static_cast<int>(fabs(min)) + static_cast<int>(fabs(max));
        int freq = GetFrequency(abs(fpar));

		if(title != wxT(""))
		{
			SetStrokeColour(*wxBLACK);
			ClearFillColour();

			dc.SetFont(boldFont);
			dc.DrawText(title, 10, 20 - dc.GetTextExtent(title).GetHeight());
		}

		// where to start drawing from        
		int originLeft = 1;
        while(max > pow((float)10, originLeft++));
        originLeft *= 7;
        
		int gap = 5;

		int barwidth;
        
        if(mode == BAR_CHART_SIMPLE)
		{
            if (keymode == CHART_LEGEND_FIXED)
            {
                barwidth = (int) ((csize - gap * data.size()) / data.size());
            }
            else
            {
                barwidth = (int) ((width - originLeft - gap * data.size() - 10) / data.size());
            }
        }
        else
        {
            if (keymode == CHART_LEGEND_FIXED)
            {
                barwidth = (int) ((csize - gap * data.size()) / (data.size() * serieLabel.size()));
            }
            else
            {
                barwidth = (int) ((width - originLeft - gap * data.size() - 10) / (data.size() * serieLabel.size()));
            }
        }

        // draw the legend/key of the chart
		if (keymode == CHART_LEGEND_FIXED)
		{
			SetStrokeColour(*wxBLACK);
			ClearFillColour();

			dc.SetFont(boldFont);
			wxString legend = _("Legend:");
			dc.DrawText(legend, originLeft + csize + 15, 25 - dc.GetTextExtent(legend).GetHeight());

			dc.SetFont(plainFont);

			if(mode == BAR_CHART_SIMPLE)
			{
				for (unsigned int i = 0; i < data.size(); i++) {
					dc.DrawText(
							data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */,
							originLeft + csize + 30,
							43 + i * 18 - dc.GetTextExtent(legend).GetHeight());
				}
			}
            else
            {
                for (unsigned int i = 0; i < serieLabel.size(); i++) {
                    dc.DrawText(
                            serieLabel[i] /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */,
                            originLeft + csize + 30,
                            43 + i * 18 - dc.GetTextExtent(legend).GetHeight());
                }
            }

			if(mode == BAR_CHART_SIMPLE)
			{
				dc.DrawRectangle(
						originLeft + csize + 10,
						10,
						width - (originLeft + csize + 15),
						(int) (20 + data.size() * 18));
			}
			else
			{
				dc.DrawRectangle(
						originLeft + csize + 10,
						10,
						width - (originLeft + csize + 15),
						(int) (20 + serieLabel.size() * 18));
			}

			// draw axis
			dc.DrawLine(originLeft - 3, height - 25, originLeft + csize + 3, height - 25);
			dc.DrawLine(originLeft, height - 22, originLeft, 22);
			// vertical axis labels
			int y = height - 25;
			int step = static_cast<int>( (height - 50) / ( (fabs(min) + fabs(max))/freq ) );
			for(int i= static_cast<int>(min); i<=max; i += freq)
			{
				// draw the label
                ClearFillColour();
                SetStrokeColour(*wxBLACK);

				wxString label = wxString::Format(wxT("%d"), i);
				dc.DrawText(label, 3, y - dc.GetTextExtent(label).GetHeight());

                // 1st entry skip grid, otherwise axis is overriden
                if(i != min)
                {
                    // draw grid
                    ClearFillColour();
                    SetStrokeColour(*wxLIGHT_GREY);

                    dc.DrawLine(originLeft + 3, y, originLeft + csize - 3, y);
                }
                y -= step;
			}


            if(mode == BAR_CHART_SERIES)
            {
                int labelOffset = (int) (((barwidth * serieLabel.size()) + gap) / 2);
                
                SetStrokeColour(*wxBLACK);
                ClearFillColour();
                
                for (unsigned int i = 0; i < data.size(); i++) {
                    wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                    dc.DrawText(
                            key,
                            originLeft + i * (barwidth * serieLabel.size()) + (i + 1) * gap + labelOffset - (dc.GetTextExtent(key).GetWidth() / 2),
                            height - 10 - dc.GetTextExtent(key).GetHeight());
                }
            }
        }
        else
        {
            SetStrokeColour(*wxBLACK);
            ClearFillColour();

            // draw axis
            dc.DrawLine(originLeft - 3, height - 25, width - 5, height - 25);
            dc.DrawLine(originLeft, height - 22, originLeft, 22);


            dc.SetFont(plainFont);

            int y = height - 25;
            int step = static_cast<int>( (height - 50) / ( (fabs(min) + fabs(max))/freq ));

            // horiz axis labels
            if(mode == BAR_CHART_SIMPLE)
            {
                int labelOffset = (barwidth + gap) / 2;
                labelOffset -= 20;
                for (unsigned int i = 0; i < data.size(); i++) {
                    wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                    dc.DrawText(
                            key,
                            originLeft + i*barwidth + (i + 1)*gap + labelOffset /* - (dc.GetTextExtent(key).GetLength() / 2) */,
                            height - 10 - dc.GetTextExtent(key).GetHeight());
                }
            }

            // vertical axis labels

            for(int i= (int) min; i<=max; i += freq)
            {
                // draw the label
                ClearFillColour();
                SetStrokeColour(*wxBLACK);

				wxString label = wxString::Format(wxT("%d"), i);
                dc.DrawText(label, 3, y - dc.GetTextExtent(label).GetHeight());

                // 1st entry skip grid, otherwise axis is overriden
                if(i != min)
                {
                    // draw grid
                    ClearFillColour();
                    SetStrokeColour(*wxLIGHT_GREY);

                    dc.DrawLine(originLeft + 3, y, width - 8, y);
                }
                y -= step;
            }

            if(mode == BAR_CHART_SERIES)
            {
                int labelOffset = (int) (((barwidth * serieLabel.size()) + gap) / 2);

                SetStrokeColour(*wxBLACK);
                ClearFillColour();

                for (unsigned int i = 0; i < data.size(); i++) {
                    wxString key = data[i].key /* + wxT(" (") + wxString::Format(wxT("%.2f"), data[i].aval) + wxT(")") */;
                    dc.DrawText(
                            key,
                            originLeft + i * (barwidth * serieLabel.size()) + (i + 1) * gap + labelOffset /* - (dc.GetTextExtent(key).GetLength() / 2) */,
                            height - 10 - dc.GetTextExtent(key).GetHeight());
                }
            }
        }

        // draw the bars
		if(mode == BAR_CHART_SIMPLE)
		{
            int zero = (int) (min * (height - 50) / (max - min));

            for (unsigned int k = 0; k < data.size(); k++)
			{
				ClearStrokeColour();
				SetFillColour(palete[k % PAL_MAX]);

                int h = static_cast<int>( data[k].aval*(height - 50)/(max - min) );

				if(h == 0)
				{
					if(data[k].aval > 0)
					{
						h = 1;
					}
					else if(data[k].aval < 0)
					{
						h = -1;
					}
				}

                if(data[k].aval >= 0)
                {
                    dc.DrawRectangle(
                            originLeft + k * barwidth + (k + 1) * gap,
                            (height - 25) + zero - h,
                            barwidth,
                            h);
                }
                else
                {
                    dc.DrawRectangle(
                            originLeft + k * barwidth + (k + 1) * gap,
                            (height - 25) + zero,
                            barwidth,
                            -h);
                }

                // key tag
                if (keymode == CHART_LEGEND_FIXED)
                {
                    dc.DrawRectangle(
                            originLeft + csize + 15,
                            33 + k * 18,
                            10,
                            10);
                }
            }
		}
        else
        {
            int zero = (int) (min * (height - 50) / (max - min));

            for (unsigned int k = 0; k < data.size(); k++)
            {
                for (unsigned int j = 0; j < serieLabel.size(); j++)
                {
                    ClearStrokeColour();
                    SetFillColour(palete[j % PAL_MAX]);

                    int h = (int) (data[k].serie[j] * (height - 50) / (max - min));

					if(h == 0)
					{
						if(data[k].aval > 0)
						{
							h = 1;
						}
						else if(data[k].aval < 0)
						{
							h = -1;
						}
					}
                    if(data[k].serie[j] >= 0)
                    {
                        dc.DrawRectangle(
                                originLeft + (k * serieLabel.size() + j) * barwidth + (k + 1)*gap,
                                (height - 25) + zero - h,
                                barwidth,
                                h);
                    }
                    else
                    {
                        dc.DrawRectangle(
                                originLeft + (k * serieLabel.size() + j) * barwidth + (k + 1)*gap,
                                (height - 25) + zero,
                                barwidth,
                                -h);
                    }

                    // key tag
                    if (keymode == CHART_LEGEND_FIXED)
                    {
                        // only once
                        if(k == 0)
                        {
                            dc.DrawRectangle(
                                    originLeft + csize + 15,
                                    33 + j * 18,
                                    10,
                                    10);
                        }
                    }
                }
            }
        }

        return true;
	};

private:
	int csize;
	int keymode;
	int mode;
};

#endif // _MM_EX_CHART_H_

