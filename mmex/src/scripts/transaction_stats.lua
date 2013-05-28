local function get_stats()
    --test table
    m_stat = {} -- TODO: provide c++ function to get stats
    y_stat = {1,2,3,4,5,6,7,8,9,10}
    for m = 1, 12, 1 do m_stat[m] = (y_stat) end
    --test end

    mmHTMLStartTable("50%")
    for m = 0, 12 , 1 do
        mmHTMLStartTableRow()
        mmHTMLTableCellMonth(m)
        for y = 1, 10, 1 do
            if m > 0 then
                mmHTMLTableCellInteger(m_stat[m][y])
            else
                mmHTMLTableHeaderCell(y)
            end
        end
        mmHTMLEndTableRow()
    end
    mmHTMLEndTable()
end
-----------------------------------------------------------
--Main function
-----------------------------------------------------------
mmHTMLReportHeader(_("Transaction Statistics"))
--mmHTMLhr
get_stats()

