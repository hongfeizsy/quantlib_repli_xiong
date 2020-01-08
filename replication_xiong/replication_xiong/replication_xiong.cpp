#include <iostream>
#include <boost/timer.hpp>
#include <ql/quantlib.hpp>

using namespace QuantLib;

int main()
{
	boost::timer timer;
	Calendar calendar = Germany(Germany::FrankfurtStockExchange);
	Date todayDate(6, July, 2009);
	Settings::instance().evaluationDate() = todayDate;
	Integer fixingDays = 2;
	Date settlementDate = calendar.advance(todayDate, fixingDays, Days);

	std::vector<std::pair<Period, boost::shared_ptr<Quote>>> depoQuotes = {
		std::make_pair(Period(1, Days), boost::make_shared<SimpleQuote>(0.00332))
	};

	std::vector<std::pair<Period, boost::shared_ptr<Quote>>> oisQuotes = {
		std::make_pair(Period(1, Weeks), boost::make_shared<SimpleQuote>(0.00452)),
		std::make_pair(Period(2, Weeks), boost::make_shared<SimpleQuote>(0.00444)),
		std::make_pair(Period(1, Months), boost::make_shared<SimpleQuote>(0.004685)),
		std::make_pair(Period(2, Months), boost::make_shared<SimpleQuote>(0.00513)),
		std::make_pair(Period(3, Months), boost::make_shared<SimpleQuote>(0.00542)),
		std::make_pair(Period(4, Months), boost::make_shared<SimpleQuote>(0.00563)),
		std::make_pair(Period(5, Months), boost::make_shared<SimpleQuote>(0.00577)),
		std::make_pair(Period(6, Months), boost::make_shared<SimpleQuote>(0.00592)),
		std::make_pair(Period(7, Months), boost::make_shared<SimpleQuote>(0.00603)),
		std::make_pair(Period(8, Months), boost::make_shared<SimpleQuote>(0.00618)),
		std::make_pair(Period(9, Months), boost::make_shared<SimpleQuote>(0.00644)),
		std::make_pair(Period(10, Months), boost::make_shared<SimpleQuote>(0.00666)),
		std::make_pair(Period(11, Months), boost::make_shared<SimpleQuote>(0.00683)),
		std::make_pair(Period(12, Months), boost::make_shared<SimpleQuote>(0.00718)),
		std::make_pair(Period(18, Months), boost::make_shared<SimpleQuote>(0.00937)),
		std::make_pair(Period(2, Years), boost::make_shared<SimpleQuote>(0.01195)),
		std::make_pair(Period(30, Months), boost::make_shared<SimpleQuote>(0.01451)),
		std::make_pair(Period(3, Years), boost::make_shared<SimpleQuote>(0.017)),
		std::make_pair(Period(4, Years), boost::make_shared<SimpleQuote>(0.02104)),
		std::make_pair(Period(5, Years), boost::make_shared<SimpleQuote>(0.02413)),
		std::make_pair(Period(6, Years), boost::make_shared<SimpleQuote>(0.02656)),
		std::make_pair(Period(7, Years), boost::make_shared<SimpleQuote>(0.02851)),
		std::make_pair(Period(8, Years), boost::make_shared<SimpleQuote>(0.03007)),
		std::make_pair(Period(9, Years), boost::make_shared<SimpleQuote>(0.03138)),
		std::make_pair(Period(10, Years), boost::make_shared<SimpleQuote>(0.03252)),
		std::make_pair(Period(11, Years), boost::make_shared<SimpleQuote>(0.03356)),
		std::make_pair(Period(12, Years), boost::make_shared<SimpleQuote>(0.03451)),
		std::make_pair(Period(15, Years), boost::make_shared<SimpleQuote>(0.03664)),
		std::make_pair(Period(20, Years), boost::make_shared<SimpleQuote>(0.03819)),
		std::make_pair(Period(25, Years), boost::make_shared<SimpleQuote>(0.03801)),
		std::make_pair(Period(30, Years), boost::make_shared<SimpleQuote>(0.03747))
	};

	DayCounter dayCounter = Actual360();
	std::vector<boost::shared_ptr<RateHelper>> eoniaInstruments;
	eoniaInstruments.push_back(boost::make_shared<DepositRateHelper>(Handle<Quote>(depoQuotes[0].second), 
		depoQuotes[0].first, fixingDays, calendar, Following, false, dayCounter));

	boost::shared_ptr<Eonia> eonia = boost::make_shared<Eonia>();
	for (int i = 0; i < oisQuotes.size(); i++) {
		eoniaInstruments.push_back(boost::make_shared<OISRateHelper>(fixingDays, oisQuotes[i].first,
			// 'telescopicValueDates' should be set to true to speed up the calculation. 'paymentLag' should be set to zero
			Handle<Quote>(oisQuotes[i].second), eonia, Handle<YieldTermStructure>(), true, 0));
	}

	boost::shared_ptr<YieldTermStructure> eoniaTermStructure =
		boost::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>(settlementDate, 
			eoniaInstruments, dayCounter, 1.0e-15);
	eoniaTermStructure->enableExtrapolation();

	RelinkableHandle<YieldTermStructure> discountingTermStructure;
	discountingTermStructure.linkTo(eoniaTermStructure);
	std::cout << "discounting factor: " << eoniaTermStructure->discount(settlementDate + 30 * Years) << std::endl;
	for (Size i = 0; i < eoniaInstruments.size(); i++) {
		Date maturityDate = eoniaInstruments[i]->maturityDate() + Period(1, Days);
		std::cout << maturityDate.weekday() << ", "
			<< maturityDate << " discount factor: "
			<< eoniaTermStructure->discount(eoniaInstruments[i]->maturityDate())
			<< std::endl;;
	}

	std::cout << "Computation time: " << timer.elapsed() << " seconds." << std::endl;
	return 0;
}

