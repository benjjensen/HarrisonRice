close all;
for m = 6 : 6
    figure();
    for r = 4 : 4
        if r < m
            n = 2^m;
            k = 0;
            for i = 0 : r
                k = k + nchoosek(m,i);
            end
            genvals = zeros(1,m);
            for numarrays = 1 : m
                genvals(1,numarrays) = 2^(numarrays - 1);
            end
            finalvals = n;
            for numcombos = 1:r
                combos = nchoosek(genvals(1,:),numcombos);
                [rows columns] = size(combos);
                for index = 1:rows
                    combossum = sum(combos(index,:));
                    finalvals = [finalvals 2^m-combossum];
                end
            end
            finalvals = sort(finalvals);
            
            weightsindex = 1;
            X = zeros(1,n+1);
            for xvals = 0 : n
                X(1,xvals + 1) = xvals;
            end
            Y = zeros(1,n+1);
            yval = k;
            index = 1;
            for yvals = 1 : n +1
                if yvals <= finalvals(1,index)
                    Y(1, yvals) = yval;
                else
                    yval = yval - 1;
                    Y(1, yvals) = yval;
                    index = index + 1;
                end
            end
            
            hold on
            plot(X,Y);
            grid on;
            title(['Security Curve for RM(' num2str(r) ',' num2str(m) ')']);
        end
    end
    hold off
end