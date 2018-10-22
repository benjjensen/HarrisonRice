function [H,exp_eqiv] = getExpectedEquivocation(mu_pmf,k,weights)
%getExpectedEquivocation finds the expected equivocation based on the
%probability distribution of mu
%   Detailed explanation goes here
[~,num_col] = size(mu_pmf);
equivocation = 0;
for col = 1:num_col
    H = weights(1,col);
    percentLeaked = 100*(k-H)/k;
    percentLeaked = percentLeaked * mu_pmf(1,col);
    equivocation = equivocation + percentLeaked;
end
    exp_eqiv = equivocation;
end

