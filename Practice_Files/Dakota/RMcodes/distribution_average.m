function [averages] = distribution_average(gc,pr_mat)
%UNTITLED5 Summary of this function goes here
%   Finds the average of number of revealed bits based on the carrier
%   probabilities of the eavesdropper
tic;
[num_rows,num_cols] = size(gc);
aa = [];
for row = 1:num_rows
    pr = [];
    for col = 1:num_cols
        if gc(row,col) ~= 0
            pr(1,col) = pr_mat(row,gc(row,col));
        end
    end
    aa(row,:) = get_dist(pr);
end
averages = aa;
toc;
end

