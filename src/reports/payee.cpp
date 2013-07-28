#include "payee.h"

#include "htmlbuilder.h"
#include "util.h"
#include "mmgraphpie.h"
#include "model/Model_Currency.h"

#include <algorithm>

mmReportPayeeExpenses::mmReportPayeeExpenses(mmCoreDB* core, const wxString& title, mmDateRange* date_range)
    : mmPrintableBase(core)
    , title_(title)
    , date_range_(date_range)
{}

mmReportPayeeExpenses::~mmReportPayeeExpenses()
{
    if (date_range_) delete date_range_;
}

wxString mmReportPayeeExpenses::getHTMLText()
{
    core_->currencyList_.LoadBaseCurrencySettings();
    //Model_Infotable::instance().GetBaseCurrencyId();

    double positiveTotal = 0.0, negativeTotal = 0.0;
    std::vector<ValuePair> valueList;
    valueList.clear();
    struct data_holder {wxString payee_name; double incomes; double expences;} line;
    std::vector<data_holder> data;

    std::map<int, std::pair<double, double> > payeeStats;
    core_->bTransactionList_.getPayeeStats(payeeStats, date_range_
        , mmIniOptions::instance().ignoreFutureTransactions_ );

    for (const auto& entry : payeeStats)
    {
        positiveTotal += entry.second.first;
        negativeTotal += entry.second.second;

        line.payee_name = core_->payeeList_.GetPayeeName(entry.first);
        line.incomes = entry.second.first;
        line.expences = entry.second.second;
        data.push_back(line);
    }

    std::stable_sort(data.begin(), data.end()
            , [] (const data_holder x, const data_holder y)
            {
                if (x.expences+x.incomes != y.expences+y.incomes) return x.expences+x.incomes < y.expences+y.incomes;
                else return x.payee_name < y.payee_name;
            }
    );

    mmHTMLBuilder hb;
    hb.init();
    hb.addHeader(2, title_);
    hb.DisplayDateHeading(date_range_->start_date(), date_range_->end_date(), date_range_->is_with_date());
    hb.startCenter();

    // Add the graph
    mmGraphPie gg;
    hb.addImage(gg.getOutputFileName());

    hb.startTable("50%");
    hb.startTableRow();
    hb.addTableHeaderCell(_("Payee"));
    hb.addTableHeaderCell(_("Incomes"), true);
    hb.addTableHeaderCell(_("Expences"), true);
    hb.addTableHeaderCell(_("Difference"), true);
    hb.endTableRow();

    for (const auto& entry : data)
    {
        hb.startTableRow();
        hb.addTableCell(entry.payee_name);
        hb.addMoneyCell(entry.incomes);
        hb.addMoneyCell(entry.expences);
        hb.addMoneyCell(entry.incomes + entry.expences);
        hb.endTableRow();

        if (entry.incomes + entry.expences < 0)
        {
            ValuePair vp;
            vp.label = entry.payee_name;
            vp.amount = entry.incomes + entry.expences;
            valueList.push_back(vp);
        }
    }

    hb.addRowSeparator(4);
    std::vector <double> totals;
    totals.push_back(positiveTotal);
    totals.push_back(negativeTotal);
    totals.push_back(positiveTotal + negativeTotal);
    hb.addTotalRow(_("Total:"), 3, totals);

    hb.endTable();
    hb.endCenter();
    hb.end();

    gg.init(valueList);
    gg.Generate(title_);

    return hb.getHTMLText();
}
