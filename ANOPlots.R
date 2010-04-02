# ANOPlots.R
# Copyright 2010 Mayank Lahiri, mlahiri@gmail.com
#
# Takes output from the reference GADM implementation and
# generates the two figures found in the AAAI-2010 paper
#
# Run it using the following command line:
#
#    R --no-save -f ANOPlots.R
#
#  (make sure the data file is in the same directory)
# The output is in "fig-ANO-cdf.eps" and "fig-cormat.eps"
#
# ################################################################
# INPUT FILE  (change the filename HERE)
#
dat<-read.table('result-enron.txt', header=T)
#
# ###############################################################
#
#
# DO NOT EDIT BELOW THIS LINE
##################################################################

# ## SETUP CODE
panel.cor <- function(x, y, digits=2, prefix="", cex.cor)
{
    usr <- par("usr"); on.exit(par(usr))
    par(usr = c(0, 1, 0, 1))
    r <- abs(cor(x, y))
    txt <- format(c(r, 0.123456789), digits=digits)[1]
    txt <- paste(prefix, txt, sep="")
    if(missing(cex.cor)) cex <- 0.8/strwidth(txt)
    test <- cor.test(x,y)
    # borrowed from printCoefmat
    text(0.5, 0.5, txt, cex = cex * r)
}
###############



# set figure margins and output
postscript("fig-ANO-cdf.eps", horizontal=FALSE, width=7, height=3, paper="special", onefile=FALSE)
par(oma=c(0,0,0,0))
par(mgp=c(1.9,0.5,0))
par(mar=c(3,2.9,1.5,0.6))
par(mfrow=c(1,2))

cdf <- ecdf(dat$ano_initial)
plot(cdf, do.points=FALSE,  xlab="Average normalized objective (ANO)", ylab=expression(Fn(ANO)), main="Initial distribution", xlim=c(0,1), lwd=2)
title("")


cdf <- ecdf(dat$ano_final)
plot(cdf, do.points=FALSE,xlab="Average normalized objective (ANO)", ylab=expression(Fn(ANO)), main="Final distribution", xlim=c(0,1), lwd=2)
title("")
med <- median(dat$ano_final)
q70 <- quantile(dat$ano_final, probs=c(0.70))
segments(-0.1,0.5, med, 0.5, lty=1, col='red', lwd=2)
segments(med,0.5,med, 0, lty=1, col='red', lwd=2)
segments(-0.1,0.7, q70, 0.7, lty=2, col='red', lwd=2)
segments(q70,0.7,q70, 0, lty=2, col='red', lwd=2)

legend("bottomright", col=c("red","red"), pch=c(-1,-1), lty=c(1,2), legend=c("Median",expression(70~percentile)),
       inset=0.00001, bg="white", bty="n")
dev.off()




# PLOT top 1% nodes correlation matrix
#
par(mfrow=c(1,1))
attach(dat)
q99 <- quantile(ano_final, probs=c(0.98))
highnodes <- subset(dat[,c(3,4,5,6,7)], dat$ano_final >= q99)

postscript("fig-cormat.eps", horizontal=FALSE, width=8, height=6, paper="special", onefile=FALSE)
par(oma=c(0,0,0,0))
par(mgp=c(1.9,0.5,0))
par(mar=c(3,2.9,1.5,0.6))

colnames(highnodes) <- c("ANO", "InDegree", "OutDegree", "Contacts", "FirstSeen")
pairs(highnodes, lower.panel=panel.smooth, upper.panel=panel.cor)
