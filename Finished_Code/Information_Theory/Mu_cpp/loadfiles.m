clear;
for i = 1:18
    filename = sprintf('mu_%d_rel', i);
    eval(sprintf('mu_%d_rel = importdata("mu_%d_rel.txt");',i,i));
    eval(sprintf('save("%s" , "%s");', filename, filename));
end