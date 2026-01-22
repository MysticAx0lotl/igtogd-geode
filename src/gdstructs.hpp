#ifndef GD_STRUCTS
#define GD_STRUCTS

struct gdObj
{
    std::string p1_id = "1"; //block is default, can be changed to spike or pit
    std::string p2_x = "0";
    std::string p3_y = "0";
    std::string p21_colorID = "0";
    std::string p24_zLayer = "0";
};

struct gdColorTrigger
{
    std::string p1_id = "899";
    std::string p2_x = "0";
    std::string p3_y = "0";
    std::string p7_red = "0";
    std::string p8_green = "0";
    std::string p9_blue = "0";
    std::string p10_duration = "0.25";
    std::string p23_channel = "1000";
    std::string remainder = ",155,1,35,1";
};

struct gdCameraObj
{
    std::string base = "1,2015,2,";
    std::string xpos = "0";
    std::string middle = ",3,2970,155,2,36,1,85,2,68,";
    std::string rotation = "180";
};

struct gdMirrorPortal
{
    std::string base = "1,";
    std::string objID = "45"; //45 = mirror start, 46 = mirror end
    std::string middle = ",2,";
    std::string xpos = "15";
    std::string remainder = ",3,45,135,1,155,2,36,1,116,1";
};

struct gdBlocksRise
{
    std::string base = "1,";
    std::string id = "23"; //set to 1915 for endBlocksRise
    std::string middle = ",2,";
    std::string xpos = "15";
    std::string remainder = ",3,2940,155,1,36,1,217,1";
};

struct gdBlocksFall
{
    std::string base = "1,";
    std::string id = "23"; //set to 1915 for endBlocksFall
    std::string middle = ",2,";
    std::string xpos = "15";
    std::string remainder = ",3,2910,155,1,36,1,217,2";
};

#endif