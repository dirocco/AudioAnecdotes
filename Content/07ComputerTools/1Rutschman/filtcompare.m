function dat = filtcompare(vec1, vec2);
f = ones(1,64);
v1 = filter(f,1,vec1);
v2 = filter(f,1,vec2);
dat = [v1; v2; v1-v2];
end
