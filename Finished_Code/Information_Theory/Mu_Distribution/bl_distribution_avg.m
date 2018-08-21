function [averages] = bl_distribution_avg(gc,pr_mat,bl)
%bl_distribution_avg Finds the average number of revealed bits as a
%probability distribution
%   Finds the average of number of revealed bits based on the carrier
%   probabilities of the eavesdropper
tic;
[num_rows,num_cols] = size(gc);
% base = floor(log2(num_cols));
% num_cols = 2^base;
aa = zeros(num_rows,bl+1);
gg = [];

for row = 1:num_rows
%     tic;
    pr = [];
    for col = 1:num_cols
        if gc(row,col) ~= 0
            pr(1,col) = pr_mat(row,gc(row,col));
        end
    end
    [~,pr_cols] = size(pr);
    base = floor(log2(pr_cols));
    pr_cols = 2^base;
    pr = pr(1:pr_cols);
    gg = get_groups(pr,bl);
    aa(row,:) = bl_get_dist(gg);
%     toc;
end
averages = aa;
toc;
end