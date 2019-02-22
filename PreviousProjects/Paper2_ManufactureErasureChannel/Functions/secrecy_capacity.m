% Created by Dakota Flanary
function [ sec_cap ] = secrecy_capacity( bob_cap,eve_cap )
%secrecy_capacity Calculates the secrecy capacity
%   If eve's capacity is greater than bob's return 0 else return bob - eve

if bob_cap < eve_cap
    sec_cap = 0;
else
    sec_cap = bob_cap - eve_cap;
end
end