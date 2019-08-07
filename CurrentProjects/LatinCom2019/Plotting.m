% Make Plots - NOT USED?

%% 1) Power Spectral Density 

clear; close all;

load('Data/Processed-Sundance-US189-25June2019.mat');

shifted = mag2db(shifted);

 % Full, normal version 
figure()
stem(shifted);
title('Unaltered');
carrierIndices = find(shifted > 35);
middleIndices = find((shifted > 4) & (shifted < 33));
xlim([0 2048]);
xlabel('Carrier Index');
ylabel('Signal strength (dB)');


 % Isolates the actual carriers
for index = 1:64
    carrierValues(index) = shifted(carrierIndices(index));
end

figure()
stem(carrierValues);
title('Carriers Only');


 % Sets all the mid-values to NaN to make a cleaner plot
temp = length(middleIndices);
for index = 1:temp
    shifted(middleIndices(index)) = nan;
end

figure()
stem(shifted)
title('Altered for Appearance');
xlim([0 2048]);
xlabel('Carrier Index');
ylabel('Signal strength (linear)');


%% 

