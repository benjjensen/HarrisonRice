%%%%%% CCDF %%%%%%%%
clear; close all;

load('Data/powerPerMilliSecondArray.mat');
temp = powerPerMilliSecondArray(1,1.266e4:end); % Cuts off the beginning b/c it is weird
temp = temp / max(temp); % Normalized to max 

% figure()
% plot(10*log10(temp))
% title('Power Array')

    % Generates a Histogram 
figure()
y = histogram(10*log10(temp));
title('Histogram')

    % Averages bins
[~, edgeVals] = size(y.BinEdges);
for i = 1:edgeVals-1
    xaxis(i) = mean(y.BinEdges(i:i+1));
end

yp = y.Values / length(temp);
   
    % Accumulates the values 
cumulative = cumsum(yp);

% figure()
% % semilogy(1-cummulative); grid on;
% plot(xaxis, 1-cumulative); grid on;
% title('CCDF');
% xlabel('Power Received');
% ylabel('Probability');
% grid on

figure()
semilogy(xaxis(1:end-1), 1-cumulative(1:end-1)); grid on;
xlabel('Power Received');
ylabel('Probability');
grid on
saveas(gcf,'Figures/CCDF','epsc');
