function [normalizedGainSquared] = normalizeChannelGain(gSquared)
%normalizeChannelGain Normalizes gSquared data by largest g

%   Given the channel gain squared values (g^2) for pwelched measurements,
%   this function solves for the largest channel gain (g), and then
%   normalizes all the gains by that value. 
    g = sqrt(gSquared);
    maxValue = max(max(g));
    normalizedG = g ./ maxValue;
    normalizedGainSquared = normalizedG.^2;
end

