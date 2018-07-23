for m = 2 : 10
    for u = 1 : m-1
        for dB = 25 :30
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
            eval(sprintf('codeInfo_%d_%d_%d.dB = %d;', dB , u, m, dB));
            
            
        end
    end
end
