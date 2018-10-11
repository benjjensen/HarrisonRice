function [pr_dist] = get_dist(pr_mat)
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
[~,n] = size(pr_mat);
probabilities = zeros(1,n);
parfor k = 0:n
    tic;
    probabilities(1,k+1) = get_values(pr_mat,n,k);
    toc;
end
pr_dist = probabilities;
end

