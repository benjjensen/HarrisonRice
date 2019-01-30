function [signal,noise] = shift_normalize_signal_noise(signal, linear_noisefloor)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
for y = 1:90
    for z= 1:345
        signal(:,y,z) = fftshift(signal(:,y,z)); % move the bad carriers to the edge
        linear_noisefloor(:,y,z) = fftshift(linear_noisefloor(:,y,z)); % move the bad carriers to the edge
    end
end

noise = linear_noisefloor(10:54,:,:); % remove the bad carriers from the noise
for y = 1:90
    for z = 1:345
        noise(:,y,z) = sum(noise(:,y,z)) ./ 45; % average the noise floor
    end
end

signal = signal(11:55,:,:); % remove the bad carriers
signal = signal ./ noise; % remove the noise from the signal
signal = sqrt(signal); % find the g's
signal = signal ./ max(max(max(signal(:,36:65,65:98)))); % find the best g in harrison's room
signal = signal .^2;
end

