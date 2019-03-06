function [cap,capacity] = gaussian_capacity(data,snr)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
capacity = (1/2) .* log2(1 + (data .* snr));
[num_columns,~] = size(data);
cap = zeros(1,num_columns);
    for col = 1:num_columns
        if ~isnan(capacity(col))
            cap(col) = sum(capacity(col,:));
        end
    end
end