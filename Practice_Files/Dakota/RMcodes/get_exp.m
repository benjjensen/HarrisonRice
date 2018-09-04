function [exp_mu] = get_exp(mu)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[num_rows,num_cols] = size(mu);
exp_mu = zeros(num_rows,1);
for row = 1:num_rows
    v = 0;
    for col = 1:num_cols
        temp = mu(row,col);
        temp = (col - 1) * temp;
        v = v + temp;
    end
    exp_mu(row,1) = v;
end

end

