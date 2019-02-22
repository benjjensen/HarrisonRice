function [signal,noise] = normalize_signal(signal, noisefloor)
% normalize_signal: Normalizes the signal
%   Averages the noise, it then divides the noise out of the signal to
%   account for the AGC (Automatic Gain Control). It then normalizes the
%   signal using the max signal value in Bob's known region. The function
%   returns g^2 and the averaged noise
% Created by Dakota Flanary/Kalin Norman


for y = 1:91
    for z = 1:345
        noise(:,y,z) = sum(noisefloor(:,y,z)) ./ 45; % average the noise floor
    end
end

signal = signal ./ noise; % normalizing to adjust for AGC
signal = sqrt(signal); % find the g's
signal = signal ./ max(max(max(signal(:,36:65,65:98)))); % find the best g in harrison's room
signal = signal .^2; % Return g^2
end

