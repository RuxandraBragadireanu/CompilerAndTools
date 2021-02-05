#pragma once
#include<string>
using namespace std;

namespace aqi {
	enum class Element{
		CO,
		PM10,
		PM25
	};

	enum class  Category{
		Good,
		Moderate,
		Unhealthy_for_sesitive_groups,
		Unhealthy,
		Very_unhealthy,
		Hazardous
	};

	class IndexBreakpoint
	{
	public:
		Category CategoryName;
		double Ilow;
		double Ihigh;

	};

	class ElementBreakpoint : public IndexBreakpoint {
	public:
		Element ElementName;
		double Clow;
		double CHigh;
		ElementBreakpoint(Element name, double Clow, double CHigh, double Ilow, double Ihigh, Category Category){
			this->ElementName = name;
			this->Clow = Clow;
			this->CHigh = CHigh;
			this->Ilow = Ilow;
			this->Ihigh = Ihigh;
			this->CategoryName = Category;
		
		}
	};

	pair<double, aqi::Category> ComputeAirQualityIndex(double concentration, Element element);
	bool CheckAirQualityIndex(double aqi, Element element, Category category);
	string ToString(Element element);
	string ToString(Category category);
	Element ToElement(string element);
	Category ToCategory(string category);

}

