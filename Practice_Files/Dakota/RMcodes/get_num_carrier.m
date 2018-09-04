function [num_carrier] = get_num_carrier(pr_mat,threshold)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,num_cols] = size(pr_mat);
num_carrier = zeros(num_rows,1);
for row = 1:num_rows
    i = 0;
    for col = 1:num_cols
        if pr_mat(row,col) > threshold
            i = i+1;
        end
    end
    base = floor(log2(i));
    i = 2^base;
    num_carrier(row) = i;
end
end

