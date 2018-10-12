%Created by Bradford Clark
%This code calculates security curves for different RM codes.
normAll = true;
normRevealed = false;
normNone = false;
normEqui = false;
showRegular = false;

if normEqui == true
    halfRatesWeightsEqui = zeros(513,9);
    halfRatesIndexesEqui = zeros(513,9);
    halfRatesNamesEqui = strings(1,9);
    countHalfEqui = 1;
end
if normRevealed == true
    halfRatesWeightsRevealed = zeros(513,9);
    halfRatesIndexesRevealed = zeros(513,9);
    halfRatesNamesRevealed = strings(1,9);
    countHalfRevealed = 1;
end
if normNone == true
    halfRatesWeightsNone = zeros(513,9);
    halfRatesIndexesNone = zeros(513,9);
    halfRatesNamesNone = strings(1,9);
    countHalfNone = 1;
end
if normAll == true
    halfRatesWeightsAll = zeros(513,9);
    halfRatesIndexesAll = zeros(513,9);
    halfRatesNamesAll = strings(1,9);
    countHalfAll = 1;
end

%closes all figure windows
close all;
for m = 1 : 20   %cycles through a range of m values
    tic
    %creates a new figure
    if showRegular
        figure();
    end
    for r = 1 : m   %cycles through a range of r values
        %         if r < m    %RM codes are only valid if this is true.
        n = 2^m;    %length of code words
        k = 0;  %initialize k = 0
        
        %computes k
        for i = 0 : r
            k = k + nchoosek(m,i);
        end
        rate = k/n;
        if rate == 0.5
            alphaValues = zeros(1,m);
            
            %determines a,b,c,ect. values
            for num = 1 : m
                alphaValues(1, num) = 2^(num-1);
            end
            
            finalValues = n;
            
            %calulates the weight heirarchy of RM code
            for numCombos = 1:r
                combos = nchoosek(alphaValues(1,:), numCombos); %finds possilbe combos of matrix
                [rows, columns] = size(combos);
                
                %sums up all of combinations
                for index = 1 : rows
                    comboSum = sum(combos(index,:));
                    finalValues = [finalValues 2^m-comboSum];   %merges matrix's
                end
            end
            
            weights = zeros(1, k);
            weights = sort(finalValues);    %orders from high to low
            
            weightsIndex = 1;
            graphWeights = zeros(1,n+1);
            
            %create y-axis values for all indexes
            for index = 1 : n + 1
                if index <= weights(1,weightsIndex)
                    graphWeights(1,index) = k - weightsIndex + 1;
                elseif index > weights(1, weightsIndex)
                    weightsIndex = weightsIndex + 1;
                    graphWeights(1,index) = k - weightsIndex + 1;
                end
            end
            
            %makes array for index of values
            graphIndexes = zeros(1,n+1);
            for j = 0 : n
                graphIndexes(1, j+1) = j;
            end
            
            if showRegular == true
                %plots curves
                hold on;
                plot(graphIndexes,graphWeights,'DisplayName',['RM(' num2str(r) ...
                    ',' num2str(m) '), k = ' num2str(k) ', rate = ' num2str(round(rate,2))]);
                grid on;
                title({['\fontsize{12}Security Curve for RM:  m = ' num2str(m) ''] ; ...
                    ['\fontsize{11}n = ' num2str(n)]});
                ylabel('Equivocation (bits)');
                xlabel ('Revealed Bits (\mu)');
                legend;
            end
            
            if rate == 0.5
                
                if normNone == true
                    halfRatesWeightsNone(1:n+1,countHalfNone) = graphWeights';
                    halfRatesIndexesNone(1:n+1,countHalfNone) = graphIndexes';
                    name = sprintf('RM(%d,%d), n = %d, k = %d',r,m, n, k);
                    halfRatesNamesNone(1,countHalfNone) = name;
                    countHalfNone = countHalfNone + 1;
                end
                
                if normEqui == true
                    halfRatesWeightsEqui(1:n+1,countHalfEqui) = graphWeights'/k*100;
                    halfRatesIndexesEqui(1:n+1,countHalfEqui) = graphIndexes';
                    name = sprintf('RM(%d,%d), n = %d, k = %d',r,m, n, k);
                    halfRatesNamesEqui(1,countHalfEqui) = name;
                    countHalfEqui = countHalfEqui + 1;
                end
                
                if normRevealed == true
                    halfRatesWeightsRevealed(1:n+1,countHalfRevealed) = graphWeights';
                    halfRatesIndexesRevealed(1:n+1,countHalfRevealed) = graphIndexes/n*100';
                    name = sprintf('RM(%d,%d), n = %d, k = %d',r,m, n, k);
                    halfRatesNamesRevealed(1,countHalfRevealed) = name;
                    countHalfRevealed = countHalfRevealed + 1;
                end
                
                if normAll == true
                    halfRatesWeightsAll(1:n+1,countHalfAll) = graphWeights'/k*100;
                    halfRatesIndexesAll(1:n+1,countHalfAll) = graphIndexes/n*100';
                    name = sprintf('RM(%d,%d), n = %d, k = %d',r,m, n, k);
                    halfRatesNamesAll(1,countHalfAll) = name;
                    countHalfAll = countHalfAll + 1;
                end
            end
        end
    end
    if showRegular
        hold off;
    end
    %     disp(m)
    toc;
    
end

if normNone == true
    figure();
    plot(halfRatesIndexesNone,halfRatesWeightsNone);
    legend(halfRatesNamesNone(1,1),halfRatesNamesNone(1,2),...
        halfRatesNamesNone(1,3),halfRatesNamesNone(1,4),...
        halfRatesNamesNone(1,5),halfRatesNamesNone(1,6),...
        halfRatesNamesNone(1,7),halfRatesNamesNone(1,8),...
        halfRatesNamesNone(1,9))
    title('Rate 0.5 Codes Same Block Length');
    xlabel('Revealed Bits (\mu)');
    ylabel('Equivocation (bits)');
    grid on;
end

if normEqui == true
    figure();
    plot(halfRatesIndexesEqui,halfRatesWeightsEqui);
    legend(halfRatesNamesEqui(1,1),halfRatesNamesEqui(1,2),...
        halfRatesNamesEqui(1,3),halfRatesNamesEqui(1,4),...
        halfRatesNamesEqui(1,4),halfRatesNamesEqui(1,6),...
        halfRatesNamesEqui(1,5),halfRatesNamesEqui(1,8),...
        halfRatesNamesEqui(1,9))
    title('Rate 0.5 Codes Same Block Length (Normalized Equivocation)');
    xlabel('Revealed Bits (\mu)');
    ylabel('Equivocation (%)');
    grid on;
end

if normRevealed == true
    figure();
    plot(halfRatesIndexesRevealed,halfRatesWeightsRevealed);
    legend(halfRatesNamesRevealed(1,1),halfRatesNamesRevealed(1,2),...
        halfRatesNamesRevealed(1,3),halfRatesNamesRevealed(1,4),...
        halfRatesNamesRevealed(1,5),halfRatesNamesRevealed(1,6),...
        halfRatesNamesRevealed(1,7),halfRatesNamesRevealed(1,8),...
        halfRatesNamesRevealed(1,9))
    title('Rate 0.5 Codes Same Block Length (Normalized Revealed Bits)');
    xlabel('Revealed Bits (%)');
    ylabel('Equivocation (bits)');
    grid on;
end

if normAll == true
    figure();
    plot(halfRatesIndexesAll,halfRatesWeightsAll);
    legend(halfRatesNamesAll(1,1),halfRatesNamesAll(1,2),...
        halfRatesNamesAll(1,3),halfRatesNamesAll(1,4),...
        halfRatesNamesAll(1,5),halfRatesNamesAll(1,6),...
        halfRatesNamesAll(1,7),halfRatesNamesAll(1,8),...
        halfRatesNamesAll(1,9))
    title('Rate 0.5 Codes Same Block Length (Normalized All)');
    xlabel('Revealed Bits (%)');
    ylabel('Equivocation (%)');
    grid on;
end