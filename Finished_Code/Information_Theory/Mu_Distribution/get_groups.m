function [gg] = get_groups(pr_mat,bl)
%UNTITLED11 Gets the potential carrier groups based on blocklength
%   Detailed explanation goes here
[~,num_cols] = size(pr_mat);
i = 0;
row = 0;
gg = [];
while i ~= num_cols
    row = row + 1;
    for j = 1:bl
        if i == num_cols
            i = 1;
        else
            i = i + 1;
        end
        gg(row,j) = pr_mat(1,i);
    end
end
end

