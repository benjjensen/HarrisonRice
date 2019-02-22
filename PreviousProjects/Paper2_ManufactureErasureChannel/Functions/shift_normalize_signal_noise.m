% Created by Dakota Flanary/Kalin Norman
function [signal,noise] = shift_normalize_signal_noise(signal, linear_noisefloor)
%shift_normalize_signal_noise: Shifts and normalizes the signal and removes
%the bad carriers
%   fftshifts the signal and noise data, removes the bad (edge) carriers, 
%   averages the noise, then normalizes the data to adjust for
%   AGC(Automatic Gain Control). It then normalizes the signal with the max
%   signal value in Bob's(Harrison's) location.  Returns g^2 and the
%   averaged noise

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
noise = noise/min(min(min(noise)));
signal = signal(11:55,:,:); % remove the bad carriers
signal = signal ./ noise; % normalizing to adjust for AGC
signal = sqrt(signal); % find the g's
signal = signal ./ max(max(max(signal(:,36:65,65:98)))); % find the best g in harrison's room
signal = signal .^2;
end

