#ifndef PT100_H__
#define PT100_H__

struct RtdValue
{
    float f;
    float resistance;
};

// http://www.intech.co.nz/products/temperature/typert/RTD-Pt100-Conversion.pdf
RtdValue rtdTable[] = {
    {-40.f, 84.27f},
    {-4.f, 92.16f},
    {32.f, 100.f},
    {50.f, 103.9f},
    {68.f, 107.79f},
    {86.f, 111.67f},
    {104.f, 115.54f},
    {122.f, 119.4f},
    {140.f, 123.24f},
    {158.f, 127.07f},
    {176.f, 130.89f},
    {194.f, 134.7f},
    {212.f, 138.5f},
    {230.f, 142.29f},
    {248.f, 146.06f},
    {266.f, 149.82f},
    {284.f, 153.58f},
    {302.f, 157.31f},
    {320.f, 161.04f},
    {338.f, 164.76f},
    {356.f, 168.46f},
    {374.f, 172.16f},
    {392.f, 175.84f},
    {410.f, 179.51f},
    {428.f, 183.17f},
    {446.f, 186.82f},
    {464.f, 190.45f},
    {482.f, 194.07f},
    {500.f, 197.69f},
    {518.f, 201.29f},
    {536.f, 204.88f},
    {554.f, 208.45f},
    {572.f, 212.02f},
    {590.f, 215.57f},
    {608.f, 219.12f},
    {626.f, 222.65f},
    {644.f, 226.17f},
    {662.f, 229.67f},
    {680.f, 233.17f},
    {698.f, 236.65f},
    {716.f, 240.13f},
    {734.f, 243.59f},
    {752.f, 247.04f},
    {770.f, 250.48f},
    {788.f, 253.9f},
    {806.f, 257.32f},
    {824.f, 260.72f},
    {842.f, 264.11f},
    {860.f, 267.49f},
    {878.f, 270.86f},
    {896.f, 274.22f},
    {914.f, 277.56f},
    {932.f, 280.9f},
};

float ResistanceToFDegree(float resistance)
{
    int count = sizeof(rtdTable) / sizeof(RtdValue);
    //std::cout << resistance << std::endl;
    if(resistance <= rtdTable[0].resistance)
        return rtdTable[0].f;

    if(resistance >= rtdTable[count - 1].resistance)
        return rtdTable[count - 1].f;

    for(int i = 1; i < count; ++i)
    {
        float lval = rtdTable[i].resistance;

        if(resistance <= lval)
        {
            float lval = rtdTable[i - 1].resistance;
            float hval = rtdTable[i].resistance;
            float htemp = rtdTable[i].f;
            float ltemp = rtdTable[i - 1].f;

            return ltemp + ((resistance - lval) * (htemp - ltemp) / (hval - lval));
        }
    }

    return 999;
}

#endif // PT100_H__
