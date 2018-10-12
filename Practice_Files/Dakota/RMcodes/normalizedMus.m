function [norm_mu] = normalizedMus(mu_pmf)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[~,num_cols] = size(mu_pmf);
norm_mu = zeros(1,num_cols);
for col = 1:num_cols
    norm_mu(col) = (col-1)/(num_cols-1);
end
end

