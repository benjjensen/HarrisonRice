function [pr_dist] = bl_get_dist(pr_mat)
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,n] = size(pr_mat);
probabilities = zeros(num_rows,(n+1));
for row = 1:num_rows
    for k = 0:n
        tic;
        row
        k
        probabilities(row,k+1) = get_values(pr_mat(row,:),n,k);
        toc;
    end
end
pr_dist = sum(probabilities)/num_rows;
end
