% Assumptions: -transmit on all carriers that aren't 0% probability
%              -if a carrier is above a given threshold, we assume you always get it
%(carrier index x dB)
tic;
load('probabilities_camacho_carriers.mat');
load('probabilities_chambers_carriers.mat');
load('probabilities_conference_carriers.mat');
load('probabilities_harrison_carriers.mat');
load('probabilities_smalley_carriers.mat');
samePlot = true;
thresholdProbability = 0.4;

% for dB = 250 : 320
%     figure(dB,'Name', sprintf('%.1f dB Limit with %.2f probability threshold',dB/10,thresholdProbability),'NumberTitle','off');
% end


for dB = 25:0.1:32
    dBIndex = (dB * 10) - 249;
    names =["camacho", "chambers", "conference", "harrison", "smalley"];
    
    
    
    
    carriers = 1:64;
    
    for i = 1:5
        for carrierIndex = 1:64
            eval(sprintf('num = probabilities_%s_carriers(carrierIndex,dBIndex);',names(1,i)));
            if num ~= 0
                carriers(carrierIndex) = 0;
            end
        end
    end
    
    numCarriersAbove(dBIndex) = 0;
    nonzeroCarriers = [];
    for i = 1:64
        if carriers(i) == 0
            numCarriersAbove(dBIndex) = numCarriersAbove(dBIndex) + 1;
            nonzeroCarriers = [nonzeroCarriers i];
        end
    end
    
    
    for j = 1:5
        
        eval(sprintf('carriersAboveThreshold_%s(dBIndex,1) = 0;',names(j)));
        for i = nonzeroCarriers
            eval(sprintf('num = probabilities_%s_carriers(i,dBIndex);',names(j)));
            if num >= thresholdProbability
                eval(sprintf('carriersAboveThreshold_%s(dBIndex,1) = carriersAboveThreshold_%s(dBIndex,1) + 1;',names(j),names(j)));
            end
        end
    end
    
    
end







for nameIndex = 1:5
    
    % max_har = mm_har(251:321,1);
    % max_sma = mm_smal(251:321,1);
    ratio = zeros(71,1);
    for j = 1:71
        eval(sprintf('ratio(j,1) = carriersAboveThreshold_%s(j,1)./numCarriersAbove(1,j);',names(nameIndex)));
    end
    
    for m = 1 : 10
        for u = 1 : m
            
            weights = RMWeightHier(u,m,false);
            
            for dB = 250 : 320
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
                carrierRate = numCarriersAbove(dB-249)*rate;
                eval(sprintf('codeInfo_%d_%d_%d.carrierRate = %d;', dB , u, m, carrierRate));
                
                percentH = 100 - percentLeaked;
                eval(sprintf('codeInfo_%d_%d_%d.percentH = %d;', dB , u, m, percentH));
                
                
            end
        end
    end
    codesPerdB = (m +1)*(m/2);
    
    for dB = 250 : 320
        workspace = who;
        eval(sprintf('outStr = regexpi(workspace, "codeInfo_%d_");', dB));
        ind = ~cellfun('isempty',outStr);
        eval(sprintf('vars%d = workspace(ind);', dB));
    end
    colors = jet(5);
    counter = 0;
    
    %%%%All on same Plot%%%%
    if (samePlot)
        %         figure();
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
    
    
    for dB = 250 : 320
        
        counter = counter + 1;
        
        %%%All on different Plots%%%%
        if (~samePlot)
            figure(dB);
            hold on;
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        
        %         %%%%UNCODED WITH MORE CARRIERS%%%%
        %         for p = 0: max_sma(dB-249)
        %             extraCarriersRate(p+1) = harrisonOnlyCarriers(dB-249, 2)+p;
        %             extraCarriersH(p+1) = 100*harrisonOnlyCarriers(dB-249, 2)/(harrisonOnlyCarriers(dB-249, 2) + p);
        %
        %             eval(sprintf('extraExtra(p+1) = "Harr: %d, Smal: %d, %.1f dB";',harrisonOnlyCarriers(dB-249, 2)+p,p,dB/10));
        %             plotextraExtra = categorical(extraExtra);
        %         end
        %         if max_sma(dB-249,1) ~= 0
        %
        %             scatter3(extraCarriersRate, extraCarriersH, plotextraExtra,[],'g', 'd','DisplayName', sprintf('UC %.1f', dB/10));
        %             clear extraExtra;
        %             clear plotextraExtra;
        %             clear extraCarriersH;
        %             clear extraCarriersRate;
        %         end
        
        
        
        
        for i = 1:codesPerdB
            eval(sprintf('name = vars%d{%d,1};',dB,i));
            %
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
                
                %                 if istrue
                %                     scatter3(plotOneRate, plotOnePercentH,plotOnedB,50,'k', '*');
                %                     istrue = false;
                %                 end
                %                 scatter(uncodedRate, uncodedSecrecy, [], 'r', 'd', 'DisplayName', 'Uncoded');
            end
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
        end
        %%%All on same Plot%%%%
        if (samePlot)
            figure(dB);
            hold on;
            scatter3(plotRate, plotPercentH,plotdB,'DisplayName', sprintf('%s %.1f dB Limit', names(nameIndex),dB/10));
            hold off;
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
    
    
    %%%All on same Plot%%%%
    %     if (samePlot)
    % %         scatter3(plotOneRate, plotOnePercentH,plotOnedB,[],'k', '*','DisplayName', 'Rate One Codes');
    %         figure(dB);
    %         title('Code Efficiency');
    %         xlabel('Throughput Rate');
    %         ylabel('Equivocation (%)');
    %         zlabel('dB Level');
    %         ylim([0 100]);
    %         xlim([0 50]);
    %         legend;
    %         grid on;
    %         hold off;
    %     end
    %%%%%%%%%%%%%%%%%%%%%%%
end

for i = 250:320
    if (samePlot)
        %         scatter3(plotOneRate, plotOnePercentH,plotOnedB,[],'k', '*','DisplayName', 'Rate One Codes');
        figure(i);
        hold on;
        title('Code Efficiency');
        xlabel('Throughput Rate');
        ylabel('Equivocation (%)');
        zlabel('dB Level');
        ylim([0 100]);
        xlim([0 50]);
        legend;
        grid on;
        hold off;
    end
end


toc;
