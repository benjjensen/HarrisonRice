close all;
colors = jet(32);
counter = 0;
x = [];
y = [];
myColors = [];
figure();
hold on;
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
%         figure(dB);
%         hold on;
        for i = 1:45
            
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            eval(sprintf('plotRate = %s.carrierRate;', name));
            eval(sprintf('plotPercentLeaked = 100 - %s.percentLeaked;', name));
%             x(counter,i) = plotRate;
%             y(counter,i) = plotPercentLeaked;
%             myColors(counter,:) = colors(33-counter,:);
            scatter3(plotRate, plotPercentLeaked,i,[],colors(33-counter,:));
        end
%         scatter(x(counter,:),y(counter,:));
%         dBTitle = round(double(dB/10),1);
%         title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
%         xlabel('Throughput Rate');
%         ylabel('Equivocation (%)');
%         ylim([0 100]);
%         xlim([0 50]);
%         grid on;
%         hold off;
    end
end

% set(gca, 'XScale', 'log');
% set(gca, 'YScale', 'log');
title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('Code Number');
% ylim([0 100]);
% xlim([0 50]);
hold off;
