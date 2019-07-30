%%%%%% CCDF %%%%%%%%
clear; close all;

load('powerPerMilliSecondArray.mat');
temp = powerPerMilliSecondArray(1, 1:220000); % Trim it to remove the ending part, when we pulled up to their car


figure()
plot(temp)
title('Power Array')

figure()
y = histogram(temp);
title('Histogram')


    % Generate a CDF by summing the histogram bins
sum = 0;
for i = 1:y.NumBins
    sum = sum + y.Values(i);
    cummulative(i) = sum;
end

cummulative = cummulative / sum;

figure()
plot(cummulative);
title('CDF');
xlabel('Power Received');
ylabel('Probability');
grid on


figure()
plot(1-cummulative);
title('CCDF');
xlabel('Power Received');
ylabel('Probability');
grid on


