function [averages] = bl_distribution_avg(gc,pr_mat,bl)
%UNTITLED5 Summary of this function goes here
%   Finds the average of number of revealed bits based on the carrier
%   probabilities of the eavesdropper
tic;
[num_rows,num_cols] = size(gc);
aa = zeros(num_rows,bl+1);
gg = [];
for row = 1:num_rows
    pr = [];
    for col = 1:num_cols
        if gc(row,col) ~= 0
            pr(1,col) = pr_mat(row,gc(row,col));
        end
    end
    gg = get_groups(pr,bl);
    aa(row,:) = bl_get_dist(gg);
end
averages = aa;
toc;
end