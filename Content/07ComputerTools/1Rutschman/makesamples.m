function makesamples(extension, format)

if (nargin < 1) 
  extension = "png";
  format = "png color small";
endif

closeplot
load -force floatdata.txt;

subrange = 1:64;

floatdata *= 16;
noisydata = floor(floatdata);
accuratedata = round(floatdata);

vector_x=[10,20,40,80,160];
vector_y=[1,2,1,0,1];
plot(vector_y);
do_output("sample1", extension, format);
plot(vector_x, vector_y);
do_output("sample2", extension, format);
closeplot

gset title "Comparison of \\`\\`fixedPointAccurate'' and \\`\\`fixedPointNoisy''"
gset noxtics
hold on
plot(accuratedata(subrange),"-@*r;fixedPointAccurate;");
plot(noisydata(subrange),"-@or;fixedPointNoisy;");
plot((noisydata-accuratedata)(subrange),"b;difference;");
do_output("difference", extension, format);
hold off
closeplot


gset yrange [-2:2]
gset nokey
gset noxtics
gset xzeroaxis
gset title "Difference between \\`\\`fixedPointNoisy'' and \\`\\`floatingPoint''"
plot((noisydata-floatdata)(subrange));
do_output("error-noisy", extension, format);

gset title "Difference between \\`\\`fixedPointAccurate'' and \\`\\`floatingPoint''"
plot((accuratedata-floatdata)(subrange));
do_output("error-accurate", extension, format);
closeplot;

gset title "Close-up view: \\`\\`fixedPointNoisy'' vs \\`\\`floatingPoint''"
gset xlabel "Data from floatingPoint"
gset ylabel "Data from fixedPointNoisy"
gset xzeroaxis
gset yzeroaxis
gset nokey
gset xrange [-2:2]
gset yrange [-2:2]
plot(floatdata,noisydata,"x");
replot "floor(x)";
do_output("rounding-noisy",extension,format);
closeplot;

gset title "Close-up view: \\`\\fixedPointAccurate'' vs \\`\\`floatingPoint''"
gset xlabel "Data point from floatingPoint"
gset ylabel "Data point from fixedPointAccurate"
gset xzeroaxis
gset yzeroaxis
gset nokey
gset xrange [-2:2];
gset yrange [-2:2];
plot(floatdata,accuratedata,"x");
replot "floor(x+0.5)";
do_output("rounding-accurate",extension, format);
closeplot;
