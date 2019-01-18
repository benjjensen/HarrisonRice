function [ sec_cap ] = secrecy_capacity( bob_cap,eve_cap )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

if bob_cap < eve_cap
    sec_cap = 0;
else
    sec_cap = bob_cap - eve_cap;
end
end

