#ifndef __BpLink_hpp__
#define __BpLink_hpp__
#include <string.h>

class BpLink
{
public:
    int ID;

    int StartPinID;
    int EndPinID;

	float Color[4];

	void SetColor(float r, float g, float b, float a = 255.0f) {
		Color[0] = r;
		Color[1] = g;
		Color[2] = b;
		Color[3] = a;
	}
    BpLink(int id, int startPinId, int endPinId)
		: ID(id)
		, StartPinID(startPinId)
		, EndPinID(endPinId)
    {}
	BpLink(const BpLink& o) { operator=(o); }

	BpLink& operator=(const BpLink& o) {
		ID = o.ID;
		StartPinID = o.StartPinID;
		EndPinID = o.EndPinID;
		memcpy(Color, o.Color, sizeof(float) * 4);
		return *this;
	}
};

#endif