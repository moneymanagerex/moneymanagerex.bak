local function add_header () 
   mmHTMLStartTableRow() 
      for i = -1, 10 - 1, 1 do 
          local y = 2013 - i 
          if i == -1 then y = nil end 
          mmHTMLBuilder("AddTableHeaderCell", y) 
      end 
   mmHTMLEndTableRow() 
end 
local function get_stats()
    --test table
	m_stat = {} -- TODO: provide c++ function to get stats
	y_stat = {1,2,3,4,5,6,7,8,9,10}
	for m = 1, 12, 1 do m_stat[m] = (y_stat) end
	--test end

	for m = 1, 12 , 1 do 
		mmHTMLStartTableRow() 
		mmHTMLTableCellMonth(m) 
		for y = 1, 10, 1 do 
			local c = m_stat[m][y]
			mmHTMLTableCellInteger(c) 
		end 
		mmHTMLEndTableRow() 
	end 
end 
----------------------------------------------------------- 
--Main function 
-----------------------------------------------------------  
mmHTMLReportHeader(_("Transaction Statistics")) 
mmHTMLStartTable("50%") 
add_header() 
get_stats() 
mmHTMLEndTable() 
