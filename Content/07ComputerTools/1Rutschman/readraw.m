function dat = readraw(filename)
	f = fopen(filename,"r");
	dat = fread(f, Inf, "short");
	fclose(f);
end
