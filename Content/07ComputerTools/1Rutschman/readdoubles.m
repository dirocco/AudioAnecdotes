function dat = readdoubles(filename)
	f = fopen(filename,"r");
	dat = fread(f, Inf, "doubles");
	fclose(f);
end
