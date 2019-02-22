function [x] = get_worst_case(pr,threshold)
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,num_cols] = size(pr);
x = zeros(num_rows,1);
for row = 1:num_rows
    v = 0;
    for col = 1:num_cols
        v = v + pr(row,col);
        if v >= threshold
            x(row) = col - 1;
            break;
        end
    end
end

end

