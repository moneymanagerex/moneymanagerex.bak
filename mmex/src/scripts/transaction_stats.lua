local function get_stats(m_stat)
    local total = 0
    mmHTMLStartTable("50%")
    for m = 0, 12 , 1 do
        mmHTMLStartTableRow()
        mmHTMLTableCellMonth(m)
        for y = 1, 10, 1 do
            if m > 0 then
                total = total + m_stat[m][y]
                mmHTMLTableCellInteger(m_stat[m][y])
            else
                mmHTMLTableHeaderCell(2014-y)
            end
        end
        mmHTMLEndTableRow()
    end
    mmHTMLEndTable()
    return total
end
-----------------------------------------------------------
--Main function
-----------------------------------------------------------
local function main(m_stat)
    mmHTMLReportHeader(_("Transaction Statistics"))
    mmHTMLhr()
    total = get_stats(m_stat)
    mmHTMLhr()
    mmHTMLaddText("Total transactions:" .. total)
end

--test table
local m_stat = {}
local y_stat = {1,2,3,4,5,6,7,8,9,10}
for m = 1, 12, 1 do m_stat[m] = (y_stat) end
--test end
main(m_stat)
