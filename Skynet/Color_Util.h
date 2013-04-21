namespace Color_Util {

    struct Pix {
        Pix() {count = 0;}
        Pix(Vec3b col) {color = col; count = 1;}
        Vec3b color;
        int count;
    };

    struct Color {
        int red, green, blue;
        std::string name;
    };

    double getMax(double d1, double d2, double d3) {
        if(d1 > d2 && d1 > d3) return d1; 
        if(d2 > d1 && d2 > d3) return d2;
        if(d3 > d1 && d3 > d2) return d3;
        if(d3 == d1 && d3 == d2) return d3;
    }

    double getMin(double d1, double d2, double d3) {
        if(d1 < d2 && d1 < d3) return d1; 
        if(d2 < d1 && d2 < d3) return d2;
        if(d3 < d1 && d3 < d2) return d3;
        if(d3 == d1 && d3 == d2) return d3;
    }

    void toHSV(double red, double green, double blue, double& hue, double& sat, double& light) {
        double min, max, delta;

        min = getMin(red, green, blue);
        max = getMax(red, green, blue);
        delta = max - min;

        light = (max + min)/2;

        if(delta != 0)
            sat = delta/max;
        else {
            sat  = 0;
            hue = -1;
            return;
        }

        if(red == max)
            hue = (green-blue)/delta;
        else if(green  == max)
            hue = 2+(blue-red)/delta;
        else
            hue = 4+(red-green)/delta;

        hue *= 60;
        if(hue < 0)
            hue += 360;
    }

    void setColorDict(std::vector<Color>& colorDict) {
        Color c;

        c.red = 255; c.green = 255; c.blue = 255; c.name = "white";
        colorDict.push_back(c);
        c.red = 255; c.green = 0; c.blue = 0; c.name = "red";
        colorDict.push_back(c);
        c.red = 0; c.green = 255; c.blue = 0; c.name = "lime";
        colorDict.push_back(c);
        c.red = 0; c.green = 0; c.blue = 255; c.name = "blue";
        colorDict.push_back(c);
        c.red = 0; c.green = 0; c.blue = 0; c.name = "black";
        colorDict.push_back(c);
        c.red = 255; c.green = 255; c.blue = 0; c.name = "yellow";
        colorDict.push_back(c);
        c.red = 0; c.green = 255; c.blue = 255; c.name = "cyan";
        colorDict.push_back(c);
        c.red = 255; c.green = 0; c.blue = 255; c.name = "magenta";
        colorDict.push_back(c);
        c.red = 192; c.green = 192; c.blue = 192; c.name = "silver";
        colorDict.push_back(c);
        c.red = 128; c.green = 128; c.blue = 128; c.name = "gray";
        colorDict.push_back(c);
        c.red = 128; c.green = 0; c.blue = 0; c.name = "maroon";
        colorDict.push_back(c);
        c.red = 128; c.green = 128; c.blue = 0; c.name = "olive";
        colorDict.push_back(c);
        c.red = 0; c.green = 128; c.blue = 0; c.name = "green";
        colorDict.push_back(c);
        c.red = 128; c.green = 0; c.blue = 128; c.name = "purple";
        colorDict.push_back(c);
        c.red = 0; c.green = 128; c.blue = 128; c.name = "teal";
        colorDict.push_back(c);
        c.red = 0; c.green = 0; c.blue = 128; c.name = "navy";
        colorDict.push_back(c);
        c.red = 255; c.green = 165; c.blue = 0; c.name = "orange";
        colorDict.push_back(c);
    }
}

