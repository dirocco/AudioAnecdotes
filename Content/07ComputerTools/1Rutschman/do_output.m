function do_output(name, extension, format)

if (nargin ~= 3)
  error("do_output: specify file, extension, and format\n");
endif

gset terminal push
eval(sprintf("gset terminal %s\n", format));
eval(sprintf("gset output \"%s.%s\"\n", name, extension));
replot
gset output
gset terminal pop

end
