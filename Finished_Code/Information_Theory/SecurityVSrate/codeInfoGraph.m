
close all;

uncodedRateGenerator;
load('mm_har.mat');
load('mm_sma.mat');
load('harrisonOnlyCarriers.mat')


max_har = mm_har(251:321,1);
max_sma = mm_smal(251:321,1);
ratio = max_sma./max_har;
for m = 1 : 10
    for u = 1 : m
        
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
codesPerdB = (m +1)*(m/2);

for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
end
colors = jet(32);
color = jet(41);
counter = 0;

%%%%All on same Plot%%%%
 figure();
 hold on;
%%%%
firstNum = nan;
secondNum = nan;
counter2 = 0;
istrue = false;

plotOnedB = zeros(1,320);
plotdB = zeros(1, 55);
plotIndex = zeros(1,55);

uncodedRate = zeros(1,32);
uncodedSecrecy = ones(1,32);
uncodedSecrecy = uncodedSecrecy.*100;

for dB = 250 : 312

    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        %%%All on different Plots%%%%
        % figure(dB);
        % hold on;
        %%%%
        
        %%%%UNCODED WITH MORE CARRIERS%%%%
        for p = 1: max_sma(dB-249)
            extraCarriersRate(p) = harrisonOnlyCarriers(counter, 2) + p;
            extraCarriersH(p) = 100*harrisonOnlyCarriers(counter, 2)/(harrisonOnlyCarriers(counter, 2) + p);
 
            eval(sprintf('extraExtra(p) = "Harr: %d, Smal: %d, %.1f dB";',harrisonOnlyCarriers(counter, 2)+p,p,dB/10));
            plotextraExtra = categorical(extraExtra);
        end
        if max_sma(dB-249,1) ~= 0
            
            scatter3(extraCarriersRate, extraCarriersH, plotextraExtra,[],'g', 'd','DisplayName', sprintf('UC %.1f', dB/10));
            clear extraExtra;
            clear plotextraExtra;
            clear extraCarriersH;
            clear extraCarriersRate;
        end
        
        
        %%%%% UNCODED %%%%%%%
            uncodedRate(counter) = harrisonOnlyCarriers(counter, 2);
            eval(sprintf('uncodeddB1(counter) = "UC %d dB";',dB/10));
            %eval(sprintf('uncodeddB1(counter) = "%.1f dB";',dB/10));
            uncodeddB = categorical(uncodeddB1);
        
        for i = 1:codesPerdB
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            
            searchName = name(14:end);
            [~, lengthName] = size(searchName);
            %%%All on different Plots%%%%
            % counter2 = 0;
            %%%%
            firstIndex = nan;
            for index = 1 :lengthName
                if searchName(index) == '_'
                    firstNum = searchName(1:index-1);
                    firstIndex = index+1;
                secondNum = searchName(index+1:end);
                end
            end
            
            if firstNum == secondNum
                istrue = true;
                counter2 = counter2 +1;
                eval(sprintf('plotOneRate(counter2) = %s.carrierRate;', name));
                eval(sprintf('plotOnePercentH(counter2) = %s.percentH;', name));
                eval(sprintf('nameU = %s.u;', name)); 
                eval(sprintf('nameM = %s.m;', name)); 
                eval(sprintf('plotOnedB1(counter2) = "RM(%d,%d): %.1f dB";',nameU,nameM,dB/10));
                plotOnedB = categorical(plotOnedB1);
            end
            
            
            eval(sprintf('plotRate(i) = %s.carrierRate;', name));
            eval(sprintf('plotPercentH(i) = %s.percentH;', name));
            eval(sprintf('NameU = %s.u;', name)); 
            eval(sprintf('NameM = %s.m;', name)); 
            eval(sprintf('plotdB1(i) = "RM(%d,%d): %.1f dB";',NameU,NameM,dB/10));
            plotdB = categorical(plotdB1);
            plotIndex(i) = i;
            
            %%%All on different Plots%%%%
%             scatter3(plotRate(i), plotPercentH(i), plotdB(i));
% 
%             if istrue
%                 scatter3(plotOneRate, plotOnePercentH,plotOnedB,50,'k', '*');
%                 istrue = false;
%             end
%             scatter(uncodedRate, uncodedSecrecy, [], 'r', 'd', 'DisplayName', 'Uncoded');
            %%%%
            
        end
        %%%All on same Plot%%%%
        scatter3(plotRate, plotPercentH,plotdB,[],colors(33-counter,:),'DisplayName', sprintf('%.1f dB Limit', dB/10));
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
scatter3(plotOneRate, plotOnePercentH,plotOnedB,[],'k', '*','DisplayName', 'Rate One Codes');
scatter3(uncodedRate, uncodedSecrecy, uncodeddB, [], 'r', 's', 'DisplayName', 'UC Perfect');        

title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('dB Level');
ylim([0 100]);
xlim([0 50]);
legend;
grid on;
hold off;
