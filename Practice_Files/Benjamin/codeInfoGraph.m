close all;
load('mm_har.mat');
load('mm_sma.mat');

max_har = mm_har(251:321,1);
max_sma = mm_smal(251:321,1);
ratio = max_sma./max_har;
for m = 2 : 10
    for u = 1 : m-1
        
        weights = RMWeightHier(u,m,false);
        
        for dB = 250 : 312
            if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
                %creates object
                eval(sprintf('codeInfo_%d_%d_%d = codeInfo;', dB , u, m));
                
                %assigns u
                eval(sprintf('codeInfo_%d_%d_%d.u = %d;', dB , u, m, u));
                
                %assigns m
                eval(sprintf('codeInfo_%d_%d_%d.m = %d;', dB , u, m, m));
                
                %assigns n
                n = 2^m;
                eval(sprintf('codeInfo_%d_%d_%d.n = %d;', dB , u, m, n));
                
                %assigns k
                k = 0;
                for i = 0 : u
                    k = k + nchoosek(m,i);
                end
                eval(sprintf('codeInfo_%d_%d_%d.k = %d;', dB , u, m, k));
                
                %assigns rate
                rate = k/n;
                eval(sprintf('codeInfo_%d_%d_%d.rate = %d;', dB , u, m, rate));
                
                %assigns dB
                RdB = dB/10;
                eval(sprintf('codeInfo_%d_%d_%d.dBLevel = %d;', dB , u, m, RdB));
                
                %assigns mu
                mu = ceil(ratio(dB-249,1) * n);
                eval(sprintf('codeInfo_%d_%d_%d.mu = %d;', dB , u, m, mu));
                
                %assigns H
                H = weights(1,mu+1);
                eval(sprintf('codeInfo_%d_%d_%d.H = %d;', dB , u, m, H));
                
                %assigns percentLeaked
                percentLeaked = 100*(k-H)/k;
                eval(sprintf('codeInfo_%d_%d_%d.percentLeaked = %d;', dB , u, m, percentLeaked));
                
                %assigns carrierRate
                carrierRate = max_har(dB-249,1)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', dB , u, m, carrierRate));
                
                percentH = 100 - percentLeaked;
                eval(sprintf('codeInfo_%d_%d_%d.percentH = %d;', dB , u, m, percentH));
                
            end
        end
    end
end

for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
end
colors = jet(32);
counter = 0;

%%%%All on same Plot%%%%
figure();
hold on;
%%%%

for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        %%%All on different Plots%%%%
%         figure(dB);
%         hold on;
        %%%%
        for i = 1:45
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            eval(sprintf('plotRate(i) = %s.carrierRate;', name));
            eval(sprintf('plotPercentH(i) = %s.percentH;', name));
            plotdB(i) = dB/10;
            plotIndex(i) = i;
            %%%All on different Plots%%%%
%             scatter3(plotRate(i), plotPercentH(i), plotIndex(i));
            %%%%
        end
        %%%All on same Plot%%%%
        scatter3(plotRate, plotPercentH,plotdB,[],colors(33-counter,:),'DisplayName', sprintf('%.1f dB Limit', dB/10));
        plot3(plotRate, plotPercentH,plotdB);
        %%%%
        
        %%%All on different Plots%%%%
%         dBTitle = round(double(dB/10),1);
%         title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
%         xlabel('Throughput Rate');
%         ylabel('Equivocation (%)');
%         ylim([0 100]);
%         xlim([0 50]);
%         grid on;
%         hold off;
        %%%%
    end
end
%%%All on same Plot%%%%
title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('dB Level');
ylim([0 100]);
xlim([0 50]);
legend;
grid on;
hold off;




% code = RMWeightHier(1,10, false);
% code(1,913-1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function output_txt = myfunction(~,event_obj)
% event_obj    Handle to event object
% output_txt   Data cursor text string (string or cell array of strings).

pos = get(event_obj,'Position');
thr = pos(1);
equ = pos(2);
dB = pos(3);

% Goes through for each @dB - calculates and compares - gets u & m
temp = true;
counter = 0;
while (temp)
    u = 0;
    m = 0;
    k = 0;
    for i = 0 : u
        k = k + nchoosek(m,i);
    end
    n = 2^m;

    rate = k/n;
    carrierRate = max_har(dB-249,1)*rate;       % make sure to put db here

    mu = ceil(ratio(dB-249,1) * n);
    H = weights(1,mu+1);
    percentLeaked = 100*(k-H)/k;
    percentH = 100 - percentLeaked;
    if ((carrierRate == thr) && (percentH == equ))
        temp = false;
        %name = 
    end
    if (counter > 45)
        temp = false;
        name = 'error';
    end
    counter = counter + 1;
end

output_txt = {['X: ',num2str(pos(1),4)],...
    ['Y: ',num2str(pos(2),4)], ...
    ['Code: ' name]};

end




