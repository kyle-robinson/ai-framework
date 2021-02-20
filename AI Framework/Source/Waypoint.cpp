#include "Waypoint.h"

std::vector<int> vecOffTrack400{ 0,1,2,3,23,8,9,10,11,12,13,14,15,16,17,18,19,39,32,31,30,
	29,28,23,22,21,20,41,42,62,81,61,100,101,121,140,160,180,200,380,217,157,197,177,178,198,
	322,323,303,302,301,281,282,262,261,242,222,203,183,163,143,124,144,125,105,104,85,86,
	66,65,29,30,32,33,49,50,51,52,72,71,70,109,129,149,169,188,168,148,128,147,167,207,
	206,186,225,171,151,131,111,91,71,112,132,152,172,173,153,174,175,89,69,110,130,150,
	170,324,304,325,286,287,307,268,288,289,269,249,250,290,271,251,231, 252,272,253,233,
	234,254,255,235,236,237,137,117,116,115,95,94,76,97,117,79,59,60,80,120,187,388,389,
	390,391,392,393,394,395,396,397,398,399,379,359,339,319,299,279,318,338,358,378,397,
	377,357,317,336,356,376,395,375,355,335,374,393,373,353,333,372,392,391,371,370,390,
	389,369,388,387,367,386,385,40,7,96,92,90,298,337,316,334,354,315,352,350,368,360,381,
	340,181,161,141,5,6,38,99,34,305,306,270,232, 139,82,102,63,43,24,4,119,220,223,223,218 };

std::vector<int> vecCheckpoint{ 311,239,159,58,54,93,176,209,264,87,47,44,162,240,362};

HRESULT	Waypoint::initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const int index )
{
	m_visible = true;
	m_isCheckpoint = false;
	m_isOnTrack = false;

	if ( std::find( vecOffTrack400.begin(), vecOffTrack400.end(), index ) == vecOffTrack400.end() )
	{
		m_isOnTrack = true;
		if ( std::find( vecCheckpoint.begin(), vecCheckpoint.end(), index ) != vecCheckpoint.end() )
		{
			setTextureName( L"Resources\\Textures\\yellow.dds" );
			m_isCheckpoint = true;
		}
		else
		{
			setTextureName( L"Resources\\Textures\\red.dds" );
		}
	}
	else {
		m_isOnTrack = false;
		float s = 10;
		m_scale = { s, s, s };
		setTextureName( L"Resources\\Textures\\blue.dds" );
	}

	HRESULT hr = DrawableGameObject::initMesh( pd3dDevice );
	return hr;
}