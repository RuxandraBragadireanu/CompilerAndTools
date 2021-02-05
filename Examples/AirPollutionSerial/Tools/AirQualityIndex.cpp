#include "AirQualityIndex.h"
#include <vector>
using namespace std;

namespace aqi {

	vector<ElementBreakpoint> ElementsBreakpoint = {
		ElementBreakpoint(Element::CO, 0, 4.4, 0, 50, Category::Good),
		ElementBreakpoint(Element::CO, 4.5, 9.4, 51, 100, Category::Moderate),
		ElementBreakpoint(Element::CO, 9.5, 12.4, 101, 150, Category::Unhealthy_for_sesitive_groups),
		ElementBreakpoint(Element::CO, 12.5, 15.4, 151, 200, Category::Unhealthy),
		ElementBreakpoint(Element::CO, 15.5, 30.4, 201, 300, Category::Very_unhealthy),
		ElementBreakpoint(Element::CO, 30.5, 40.4, 301, 400, Category::Hazardous),
		ElementBreakpoint(Element::PM25, 0, 12, 0, 50, Category::Good),
		ElementBreakpoint(Element::PM25, 12.1, 35.4, 51, 100, Category::Moderate),
		ElementBreakpoint(Element::PM25, 35.5, 55.4, 101, 150, Category::Unhealthy_for_sesitive_groups),
		ElementBreakpoint(Element::PM25, 55.5, 130.4, 151, 200, Category::Unhealthy),
		ElementBreakpoint(Element::PM25, 150.5, 250.4, 201, 300, Category::Very_unhealthy),
		ElementBreakpoint(Element::PM25, 250.5, 350.4, 301, 400, Category::Hazardous),
		ElementBreakpoint(Element::PM10, 0, 54, 0, 50, Category::Good),
		ElementBreakpoint(Element::PM10, 55, 154, 51, 100, Category::Moderate),
		ElementBreakpoint(Element::PM10, 155, 254, 101, 150, Category::Unhealthy_for_sesitive_groups),
		ElementBreakpoint(Element::PM10, 255, 354, 151, 200, Category::Unhealthy),
		ElementBreakpoint(Element::PM10, 355, 424, 201, 300, Category::Very_unhealthy),
		ElementBreakpoint(Element::PM10, 425, 504, 301, 400, Category::Hazardous),
	};


	pair<double, aqi::Category> ComputeAirQualityIndex(double concentration, Element element) 
	{
		auto iter = std::find_if(ElementsBreakpoint.begin(), ElementsBreakpoint.end(),
									[&element, &concentration](ElementBreakpoint eb)
									{
										return (eb.ElementName == element && eb.Clow <= concentration && eb.CHigh >= concentration);
									});

		if (iter == ElementsBreakpoint.end()) {
			return make_pair(NULL, Category::Good);
		}

		ElementBreakpoint eb = *iter;
		double aqi = (eb.Ihigh - eb.Ilow) / (eb.CHigh - eb.Clow) * (concentration - eb.Clow) + eb.Ilow;

		return make_pair(aqi, eb.CategoryName);

	}

	bool CheckAirQualityIndex(double aqi, Element element, Category category)
	{
		auto iter = std::find_if(ElementsBreakpoint.begin(), ElementsBreakpoint.end(),
			[&aqi, &element, &category](ElementBreakpoint eb)
		{
			return (eb.ElementName == element && eb.CategoryName == category && eb.Ilow <= aqi && eb.Ihigh >= aqi );
		});

		if (iter == ElementsBreakpoint.end()) {
			return false;
		}

		return true;
	}

	string ToString(Element element)
	{
		switch (element)
		{
			case Element::CO:  
				return "CO";
			case Element::PM10:   
				return "PM10";
			case Element::PM25: 
				return "PM25";
			default:     
				return "[Unknown Element]";
		}
	}

	string ToString(Category category)
	{
		switch (category)
		{
			case Category::Good:  
				return "Good";
			case Category::Hazardous:
				return "Hazardous";
			case Category::Moderate:
				return "Moderate";
			case Category::Unhealthy:
				return "Unhealthy";
			case Category::Unhealthy_for_sesitive_groups:
				return "Unhealthy for sesitive groups";
			case Category::Very_unhealthy:
				return "Very unhealthy";
			default:     
				return "[Unknown Category]";
		}
	}


	Element ToElement(string element)
	{
		if (element == "CO") {
			return Element::CO;
		}
		else if (element == "PM10") {
			return Element::PM10;
		}
		else if (element == "PM25") {
			return Element::PM25;
		}
		
		return Element::CO;
	}

	Category ToCategory(string category)
	{
		if (category == "Good") {
			return Category::Good;
		}
		else if (category == "Hazardous") {
			return Category::Hazardous;
		}
		else if (category == "Moderate") {
			return Category::Moderate;
		}
		else if (category == "Unhealthy") {
			return Category::Unhealthy;
		}
		else if (category == "Unhealthy for sesitive groups") {
			return Category::Unhealthy_for_sesitive_groups;
		}
		else if (category == "Very unhealthy") {
			return Category::Very_unhealthy;
		}
		
		return Category::Good;
		
	}


}