tic;
close all;

samePlot = true;
load('mm_har.mat');
load('mm_sma.mat');
load('harrisonOnlyCarriers.mat')

max_har = mm_har(251:321,1);
max_sma = mm_smal(251:321,1);
ratio = max_sma./max_har;

colors = jet(32);
color = jet(41);

%%%%%%%%%%Hamming Codes%%%%%%%%%%
for m = 2 : 10
    weightsHam = HammingWeightHier(m,false);
    for dB = 250 : 312
        if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
            %creates object
            eval(sprintf('codeInfoHamming_%d_%d = codeInfo;', dB , m));
            
            %assigns u
            eval(sprintf('codeInfoHamming_%d_%d.u = 0;', dB , m));
            
            %assigns m
            eval(sprintf('codeInfoHamming_%d_%d.m = %d;', dB, m, m));
            
            %assigns n
            n = (2^m)-1;
            eval(sprintf('codeInfoSHamming_%d_%d.n = %d;', dB, m, n));
            
            %assigns k
            k = m;
            eval(sprintf('codeInfoHamming_%d_%d.k = %d;', dB, m, k));
            
            %assigns rate
            rate = k/n;
            eval(sprintf('codeInfoHamming_%d_%d.rate = %d;', dB, m, rate));
            
            %assigns dB
            RdB = dB/10;
            eval(sprintf('codeInfoHamming_%d_%d.dBLevel = %d;', dB, m, RdB));
            
            %assigns mu
            mu = ceil(ratio(dB-249,1) * n);
            eval(sprintf('codeInfoHamming_%d_%d.mu = %d;', dB, m, mu));
            
            %assigns H
            H = weightsHam(1,mu+1);
            eval(sprintf('codeInfoHamming_%d_%d.H = %d;', dB, m, H));
            
            %assigns percentLeaked
            percentLeaked = 100*(k-H)/k;
            eval(sprintf('codeInfoHamming_%d_%d.percentLeaked = %d;', dB, m, percentLeaked));
            
            %assigns carrierRate
            carrierRate = max_har(dB-249,1)*rate;
            eval(sprintf('codeInfoHamming_%d_%d.carrierRate = %d;', dB, m, carrierRate));
            
            percentH = 100 - percentLeaked;
            eval(sprintf('codeInfoHamming_%d_%d.percentH = %d;', dB, m, percentH));
            
        end
    end
    
end
codesPerdBHam = 9;
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfoHamming_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
end
%%%%All on same Plot%%%%
if (samePlot)
    figure();
    hold on;
end
%%%%%%%%%%%%%%%%%%%%%%%%
firstNum = nan;
counter = 0;
istrue = false;
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        %%%All on different Plots%%%%
        if (~samePlot)
            figure('Name',sprintf('Hamming Codes at %d dB',dB),'NumberTitle','off');
            hold on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        for i = 1:codesPerdBHam
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            
            %%%All on different Plots%%%%
            if (~samePlot)
                counter2 = 0;
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            %
            %             searchName = name(14:end);
            %             [~, lengthName] = size(searchName);
            %             %%%All on different Plots%%%%
            %             %%%%
            %             firstIndex = nan;
            %             for index = 1 :lengthName
            %                 if searchName(index) == '_'
            %                     firstNum = searchName(1:index-1);
            %                 end
            %             end
            %
            eval(sprintf('plotRateHam(i) = %s.carrierRate;', name));
            eval(sprintf('plotPercentHHam(i) = %s.percentH;', name));
            eval(sprintf('NameM = %s.m;', name));
            eval(sprintf('plotdB1Ham(i) = "Hamming(%d): %.1f dB";',NameM,dB/10));
            plotdBHam = categorical(plotdB1Ham);
            plotIndexHam(i) = i;
            
            %%%All on different Plots%%%%
            if (~samePlot)
                scatter3(plotRateHam(i), plotPercentHHam(i), plotdBHam(i));
                
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            
            
        end
        %%%All on same Plot%%%%
        if (samePlot)
            scatter3(plotRateHam, plotPercentHHam,plotdBHam,[],colors(33-counter,:),'^','DisplayName', sprintf('%.1f dB Limit', dB/10));
        end
        %%%%%%%%%%%%%%%%%%%%%%%
        
        
        %%%All on different Plots%%%%
        if (~samePlot)
            dBTitle = round(double(dB/10),1);
            title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
            xlabel('Throughput Rate');
            ylabel('Equivocation (%)');
            ylim([0 100]);
            xlim([0 50]);
            grid on;
            hold off;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    end
end


%%%%%%%%%%Simplex Codes%%%%%%%%%%
for m = 2 : 10
    weightsSimp = SimplexWeightHier(m,false);
    for dB = 250 : 312
        if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
            %creates object
            eval(sprintf('codeInfoSimplex_%d_%d = codeInfo;', dB , m));
            
            %assigns u
            eval(sprintf('codeInfoSimplex_%d_%d.u = 0;', dB , m));
            
            %assigns m
            eval(sprintf('codeInfoSimplex_%d_%d.m = %d;', dB, m, m));
            
            %assigns n
            n = (2^m)-1;
            eval(sprintf('codeInfoSimplex_%d_%d.n = %d;', dB, m, n));
            
            %assigns k
            k = n-m;
            eval(sprintf('codeInfoSimplex_%d_%d.k = %d;', dB, m, k));
            
            %assigns rate
            rate = k/n;
            eval(sprintf('codeInfoSimplex_%d_%d.rate = %d;', dB, m, rate));
            
            %assigns dB
            RdB = dB/10;
            eval(sprintf('codeInfoSimplex_%d_%d.dBLevel = %d;', dB, m, RdB));
            
            %assigns mu
            mu = ceil(ratio(dB-249,1) * n);
            eval(sprintf('codeInfoSimplex_%d_%d.mu = %d;', dB, m, mu));
            
            %assigns H
            H = weightsSimp(1,mu+1);
            eval(sprintf('codeInfoSimplex_%d_%d.H = %d;', dB, m, H));
            
            %assigns percentLeaked
            percentLeaked = 100*(k-H)/k;
            eval(sprintf('codeInfoSimplex_%d_%d.percentLeaked = %d;', dB, m, percentLeaked));
            
            %assigns carrierRate
            carrierRate = max_har(dB-249,1)*rate;
            eval(sprintf('codeInfoSimplex_%d_%d.carrierRate = %d;', dB, m, carrierRate));
            
            percentH = 100 - percentLeaked;
            eval(sprintf('codeInfoSimplex_%d_%d.percentH = %d;', dB, m, percentH));
            
        end
    end
    
end
codesPerdBSimp = 9;
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfoSimplex_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
end
%%%%All on same Plot%%%%
if (samePlot)
    hold on;
end
%%%%%%%%%%%%%%%%%%%%%%%%
firstNum = nan;
counter = 0;
istrue = false;
for dB = 250 : 312
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        %%%All on different Plots%%%%
        if (~samePlot)
            figure('Name',sprintf('Simplex Codes at %d dB',dB),'NumberTitle','off');
            hold on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        for i = 1:codesPerdBSimp
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            
            %%%All on different Plots%%%%
            if (~samePlot)
                counter2 = 0;
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            %
            %             searchName = name(14:end);
            %             [~, lengthName] = size(searchName);
            %             %%%All on different Plots%%%%
            %             %%%%
            %             firstIndex = nan;
            %             for index = 1 :lengthName
            %                 if searchName(index) == '_'
            %                     firstNum = searchName(1:index-1);
            %                 end
            %             end
            %
            eval(sprintf('plotRateSimp(i) = %s.carrierRate;', name));
            eval(sprintf('plotPercentHSimp(i) = %s.percentH;', name));
            eval(sprintf('NameM = %s.m;', name));
            eval(sprintf('plotdB1Simp(i) = "Simplex(%d): %.1f dB";',NameM,dB/10));
            plotdBSimp = categorical(plotdB1Simp);
            plotIndexSimp(i) = i;
            
            %%%All on different Plots%%%%
            if (~samePlot)
                scatter3(plotRateSimp(i), plotPercentHSimp(i), plotdBSimp(i));
                
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            
            
        end
        %%%All on same Plot%%%%
        if (samePlot)
            scatter3(plotRateSimp, plotPercentHSimp,plotdBSimp,[],colors(33-counter,:),'x','DisplayName', sprintf('%.1f dB Limit', dB/10));
        end
        %%%%%%%%%%%%%%%%%%%%%%%
        
        
        %%%All on different Plots%%%%
        if (~samePlot)
            dBTitle = round(double(dB/10),1);
            title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
            xlabel('Throughput Rate');
            ylabel('Equivocation (%)');
            ylim([0 100]);
            xlim([0 50]);
            grid on;
            hold off;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    end
end

%%%%%%%%%%RM Codes%%%%%%%%%%
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

counter = 0;

%%%%All on same Plot%%%%
if (samePlot)
    hold on;
end
%%%%%%%%%%%%%%%%%%%%%%%%

firstNum = nan;
secondNum = nan;
counter2 = 0;
istrue = false;

plotOnedB = zeros(1,320);
plotdB = zeros(1, 55);
plotIndex = zeros(1,55);


for dB = 250 : 312
    
    if max_har(dB-249,1)- max_har(dB-248,1) ~= 0
        counter = counter + 1;
        
        %%%All on different Plots%%%%
        if (~samePlot)
            figure('Name',sprintf('RM Codes at %d dB',dB),'NumberTitle','off');
            hold on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        
        %%%%UNCODED WITH MORE CARRIERS%%%%
        for p = 0: max_sma(dB-249)
            extraCarriersRate(p+1) = harrisonOnlyCarriers(dB-249, 2)+p;
            extraCarriersH(p+1) = 100*harrisonOnlyCarriers(dB-249, 2)/(harrisonOnlyCarriers(dB-249, 2) + p);
            
            eval(sprintf('extraExtra(p+1) = "Harr: %d, Smal: %d, %.1f dB";',harrisonOnlyCarriers(dB-249, 2)+p,p,dB/10));
            plotextraExtra = categorical(extraExtra);
        end
        if max_sma(dB-249,1) ~= 0
            
%             scatter3(extraCarriersRate, extraCarriersH, plotextraExtra,[],'g', 'd','DisplayName', sprintf('UC %.1f', dB/10));
            clear extraExtra;
            clear plotextraExtra;
            clear extraCarriersH;
            clear extraCarriersRate;
        end
        
        
        
        
        for i = 1:codesPerdB
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            
            searchName = name(14:end);
            [~, lengthName] = size(searchName);
            
            %%%All on different Plots%%%%
            if (~samePlot)
                counter2 = 0;
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
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
            if (~samePlot)
                scatter3(plotRate(i), plotPercentH(i), plotdB(i));
                
                if istrue
                    scatter3(plotOneRate, plotOnePercentH,plotOnedB,50,'k', '*');
                    istrue = false;
                end
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
        end
        %%%All on same Plot%%%%
        if (samePlot)
            scatter3(plotRate, plotPercentH,plotdB,[],colors(33-counter,:),'DisplayName', sprintf('%.1f dB Limit', dB/10));
        end
        %%%%%%%%%%%%%%%%%%%%%%%
        
        
        %%%All on different Plots%%%%
        if (~samePlot)
            dBTitle = round(double(dB/10),1);
            title(sprintf('Code Efficiency with %.1f dB Limit',dBTitle));
            xlabel('Throughput Rate');
            ylabel('Equivocation (%)');
            ylim([0 100]);
            xlim([0 50]);
            grid on;
            hold off;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    end
end


%%%All on same Plot%%%%
if (samePlot)
    scatter3(plotOneRate, plotOnePercentH,plotOnedB,[],'k', '*','DisplayName', 'Rate One Codes');
    
    title('Code Efficiency');
    xlabel('Throughput Rate');
    ylabel('Equivocation (%)');
    zlabel('dB Level');
    ylim([0 100]);
    xlim([0 50]);
    %         legend;
    grid on;
    hold off;
end
%%%%%%%%%%%%%%%%%%%%%%%
toc;