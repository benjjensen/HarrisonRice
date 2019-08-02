%%%%%% CCDF %%%%%%%%
clear; close all;

load('powerPerMilliSecondArray.mat');
temp = powerPerMilliSecondArray(1,1.266e4:end);
temp = temp / max(temp);

% figure()
% plot(10*log10(temp))
% title('Power Array')

figure()
y = histogram(10*log10(temp));
title('Histogram')

[~, edgeVals] = size(y.BinEdges);
for i = 1:edgeVals-1
    xaxis(i) = mean(y.BinEdges(i:i+1));
end

yp = y.Values / length(temp);
   

cumulative = cumsum(yp);

% figure()
% plot(xaxis, cumulative);
% title('CDF');
% xlabel('Power Received');
% ylabel('Probability');
% grid on

figure()
% semilogy(1-cummulative); grid on;
plot(xaxis, 1-cumulative); grid on;
title('CCDF');
xlabel('Power Received');
ylabel('Probability');
grid on

figure()
semilogy(xaxis(1:end-1), 1-cumulative(1:end-1)); grid on;
% plot(1-cummulative); grid on;
% title('CCDF (semilogy)');
xlabel('Power Received');
ylabel('Probability');
grid on
saveas(gcf,'Figures/CCDF','epsc');

% Normalized it so that 0 dB power is when the vans are right next to one
% another. (dB relative to when they are side by side)