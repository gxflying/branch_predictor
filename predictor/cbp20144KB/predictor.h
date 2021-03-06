#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"

#include <inttypes.h>
#include <math.h>
//#define PRINTSIZE //uncomment to get the predictor size
#define LOOPPREDICTOR // option to use the loop predictor

#define NNN 1 // allocates (NNN+1)  allocated on a TAGE misprediction

#define LOGB 12     // log of number of entries in bimodal predictor
#define HYSTSHIFT 2 // shift on the hysteris bits

#define SFWIDTH 6 // statistical filter counter width
#define NHIST 10
int m[NHIST + 1] =    {0, 4, 9, 13, 24, 37, 53, 91, 145, 226, 359}; // history lengths
int TB[NHIST + 1] =   {0, 7, 7, 7, 8, 9, 10, 10, 11, 13, 13};      // tag width for the different tagged tables
int logg[NHIST + 1] = {0, 8, 8, 8, 8, 8, 7, 8, 7, 6, 6};         // log of number entries of the different tagged tables

//LOOP PREDICTOR PARAMETERs
#define LOGL 4
#define WIDTHNBITERLOOP 10 // we predict only loops with less than 1K iterations
#define LOOPTAG 10         //tag width in the loop predictor

#define PHISTWIDTH 27 // width of the path history
#define CONFWIDTH 7   // width of the choser counter

#define UWIDTH 2 //width of the useful counter
#define CWIDTH 3 // predictor counter width on the TAGE tagged tables

#define HISTBUFFERLENGTH 4096 // we use a 4K entries history buffer to store the branch history
uint8_t ghist[HISTBUFFERLENGTH];
int ptghist;
// just for managing the history

#define LOGSIZEUSEALT 5
#define SIZEUSEALT (1 << (LOGSIZEUSEALT))
#define INDUSEALT (PC & (SIZEUSEALT - 1))
int8_t use_alt_on_na[SIZEUSEALT][2];
int8_t AllocCount;
//for deciding to allocate one or two entries on a misprediction

bool HighConf; // is TAGE high confidence ?
int8_t BIM;

// utility class for index computation
// this is the cyclic shift register for folding
// a long global history into a smaller number of bits; see P. Michaud's PPM-like predictor at CBP-1
class folded_history
{
public:
  unsigned comp;
  int CLENGTH;
  int OLENGTH;
  int OUTPOINT;

  folded_history()
  {
  }

  void init(int original_length, int compressed_length, int N)
  {
    comp = 0;
    OLENGTH = original_length;
    CLENGTH = compressed_length;
    OUTPOINT = OLENGTH % CLENGTH;
  }

  void update(uint8_t *h, int PT)
  {
    comp = (comp << 1) ^ h[PT & (HISTBUFFERLENGTH - 1)];
    comp ^= h[(PT + OLENGTH) & (HISTBUFFERLENGTH - 1)] << OUTPOINT;
    comp ^= (comp >> CLENGTH);
    comp = (comp) & ((1 << CLENGTH) - 1);
  }
};

#ifdef LOOPPREDICTOR
class lentry //loop predictor entry
{
public:
  uint16_t NbIter;      //10 bits
  uint8_t confid;       // 3 bits
  uint16_t CurrentIter; // 10 bits
  uint16_t TAG;         // 10 bits
  uint8_t age;          //3 bits
  bool dir;             // 1 bit

  //37 bits per entry
  lentry()
  {
    confid = 0;
    CurrentIter = 0;
    NbIter = 0;
    TAG = 0;
    age = 0;
    dir = false;
  }
};
#endif

class bentry // TAGE bimodal table entry
{
public:
  int8_t hyst;
  int8_t pred;

  bentry()
  {
    pred = 0;

    hyst = 1;
  }
};
class gentry // TAGE global table entry
{
public:
  int8_t ctr;
  uint tag;
  int8_t u;

  gentry()
  {
    ctr = 0;
    tag = 0;
    u = 0;
  }
};

int TICK; //control counter for the resetting of useful counters

long long phist;                   //path history
folded_history ch_i[NHIST + 1];    //utility for computing TAGE indices
folded_history ch_t[2][NHIST + 1]; //utility for computing TAGE tags

//For the TAGE predictor
bentry *btable;            //bimodal TAGE table
gentry *gtable[NHIST + 1]; // tagged TAGE tables

//The statistical filter
#define LOGSTATISTIC 6
#define STATTAGWIDTH 7
#define pcstat ((pc) & ((1 << (2 * (LOGSTATISTIC - 2))) - 1))
#define pctag ((pc >> (LOGSTATISTIC - 2)) & ((1 << STATTAGWIDTH) - 1))
bool HitStatistic, IsStatistic;
bool pred_Statistic;
gentry *Statistic;
int indexStatistic;

////////////////////

// variables for internal management
int GI[NHIST + 1];    // indexes to the different tables are computed only once
uint GTAG[NHIST + 1]; // tags for the different tables are computed only once
int BI;               // index of the bimodal table
bool pred_taken;      // prediction
bool alttaken;        // alternate  TAGEprediction
bool tage_pred;       // TAGE prediction
bool LongestMatchPred;
int HitBank; // longest matching bank
int AltBank; // alternate matching bank
int Seed;    // for the pseudo-random number generator
bool pred_inter;

#ifdef LOOPPREDICTOR
lentry *ltable; //loop predictor table
//variables for the loop predictor
bool predloop; // loop predictor prediction
int LIB;
int LI;
int LHIT;        //hitting way in the loop predictor
int LTAG;        //tag on the loop predictor
bool LVALID;     // validity of the loop predictor prediction
int8_t WITHLOOP; // counter to monitor whether or not loop prediction is beneficial

#endif

int predictorsize()
{
  int STORAGESIZE = 0;

  for (int i = 1; i <= NHIST; i += 1)
  {
    STORAGESIZE += (1 << (logg[i])) * (CWIDTH + UWIDTH + TB[i]);
  }

  STORAGESIZE += (1 << LOGB) + (1 << (LOGB - HYSTSHIFT)); //the bimodal
  STORAGESIZE += 2 * (SIZEUSEALT)*4;
  STORAGESIZE += 10 + 3; //TICK and AllocCount

#ifdef PRINTSIZE
  fprintf(stderr, " (TAGE %d) ", STORAGESIZE);
#endif

#ifdef LOOPPREDICTOR
  STORAGESIZE += (1 << LOGL) * (2 * WIDTHNBITERLOOP + LOOPTAG + 4 + 4 + 1);
#endif

  STORAGESIZE += (1 << (LOGSTATISTIC)) * (STATTAGWIDTH + SFWIDTH);

  STORAGESIZE += m[NHIST];
  STORAGESIZE += PHISTWIDTH;

#ifdef PRINTSIZE
  fprintf(stderr, " (TOTAL %d) ", STORAGESIZE);
#endif

  return (STORAGESIZE);
}

class PREDICTOR
{
public:
  PREDICTOR(void)
  {
    predictorsize();
    reinit();
  }
  void reinit()
  {

#ifdef LOOPPREDICTOR
    ltable = new lentry[1 << (LOGL)];
#endif

    for (int i = 0; i <= NHIST; i++)
    {
      gtable[i] = new gentry[1 << (logg[i])];
    }

    btable = new bentry[1 << LOGB];
    Statistic = new gentry[(1 << LOGSTATISTIC)];
    for (int i = 1; i <= NHIST; i++)
    {
      ch_i[i].init(m[i], (logg[i]), i - 1);
      ch_t[0][i].init(ch_i[i].OLENGTH, TB[i], i);
      ch_t[1][i].init(ch_i[i].OLENGTH, TB[i] - 1, i + 2);
    }
#ifdef LOOPPREDICTOR
    LVALID = false;
    WITHLOOP = -1;
#endif
    Seed = 0;

    TICK = 0;
    phist = 0;
    Seed = 0;

    for (int i = 0; i < HISTBUFFERLENGTH; i++)
      ghist[0] = 0;
    ptghist = 0;

    for (int i = 0; i < (1 << LOGB); i++)
    {
      btable[i].pred = 0;
      btable[i].hyst = 1;
    }

    for (int i = 0; i < SIZEUSEALT; i++)
    {
      use_alt_on_na[i][0] = 0;
      use_alt_on_na[i][1] = 0;
    }

    TICK = 0;
    ptghist = 0;
    phist = 0;
  }

  // index function for the bimodal table

  int bindex(uint32_t PC)
  {
    return ((PC) & ((1 << (LOGB)) - 1));
  }

  // the index functions for the tagged tables uses path history as in the OGEHL predictor
  //F serves to mix path history: not very important impact

  int F(long long A, int size, int bank)
  {
    int A1, A2;
    A = A & ((1 << size) - 1);
    A1 = (A & ((1 << logg[bank]) - 1));
    A2 = (A >> logg[bank]);
    A2 =
        ((A2 << bank) & ((1 << logg[bank]) - 1)) + (A2 >> (logg[bank] - bank));
    A = A1 ^ A2;
    A = ((A << bank) & ((1 << logg[bank]) - 1)) + (A >> (logg[bank] - bank));
    return (A);
  }

  // gindex computes a full hash of PC, ghist and phist
  int gindex(unsigned int PC, int bank, long long hist,
             folded_history *ch_i)
  {
    int index;
    int M = (m[bank] > PHISTWIDTH) ? PHISTWIDTH : m[bank];
    index =
        PC ^ (PC >> (abs(logg[bank] - bank) + 1)) ^ ch_i[bank].comp ^ F(hist, M, bank);
    return (index & ((1 << (logg[bank])) - 1));
  }

  //  tag computation
  uint16_t gtag(unsigned int PC, int bank, folded_history *ch0,
                folded_history *ch1)
  {
    int tag = PC ^ ch0[bank].comp ^ (ch1[bank].comp << 1);
    return (tag & ((1 << TB[bank]) - 1));
  }

  // up-down saturating counter
  void ctrupdate(int8_t &ctr, bool taken, int nbits)
  {
    if (taken)
    {
      if (ctr < ((1 << (nbits - 1)) - 1))
        ctr++;
    }
    else
    {
      if (ctr > -(1 << (nbits - 1)))
        ctr--;
    }
  }

#ifdef LOOPPREDICTOR
  int lindex(uint32_t PC)
  {
    return ((PC & ((1 << (LOGL - 2)) - 1)) << 2);
  }

//loop prediction: only used if high confidence
//skewed associative 4-way
//At fetch time: speculative
#define CONFLOOP 15

  bool getloop(uint32_t PC)
  {
    LHIT = -1;

    LI = lindex(PC);
    LIB = ((PC >> (LOGL - 2)) & ((1 << (LOGL - 2)) - 1));
    LTAG = (PC >> (LOGL - 2)) & ((1 << 2 * LOOPTAG) - 1);
    LTAG ^= (LTAG >> LOOPTAG);
    LTAG = (LTAG & ((1 << LOOPTAG) - 1));

    for (int i = 0; i < 4; i++)
    {
      int index = (LI ^ ((LIB >> i) << 2)) + i;

      if (ltable[index].TAG == LTAG)
      {
        LHIT = i;
        LVALID = ((ltable[index].confid == CONFLOOP) || (ltable[index].confid * ltable[index].NbIter > 128));
        if (ltable[index].CurrentIter + 1 == ltable[index].NbIter)
          return (!(ltable[index].dir));
        else
          return ((ltable[index].dir));
      }
    }

    LVALID = false;
    return (false);
  }

  void loopupdate(uint32_t PC, bool Taken, bool ALLOC)
  {
    if (LHIT >= 0)
    {
      int index = (LI ^ ((LIB >> LHIT) << 2)) + LHIT;
      //already a hit
      if (LVALID)
      {
        if (Taken != predloop)
        {
          // free the entry
          ltable[index].NbIter = 0;
          ltable[index].age = 0;
          ltable[index].confid = 0;
          ltable[index].CurrentIter = 0;
          return;
        }
        else if ((predloop != tage_pred) || ((MYRANDOM() & 7) == 0))
          if (ltable[index].age < CONFLOOP)
            ltable[index].age++;
      }

      ltable[index].CurrentIter++;
      ltable[index].CurrentIter &= ((1 << WIDTHNBITERLOOP) - 1);
      //loop with more than 2** WIDTHNBITERLOOP iterations are not treated correctly; but who cares :-)
      if (ltable[index].CurrentIter > ltable[index].NbIter)
      {
        ltable[index].confid = 0;
        ltable[index].NbIter = 0;
        //treat like the 1st encounter of the loop
      }
      if (Taken != ltable[index].dir)
      {
        if (ltable[index].CurrentIter == ltable[index].NbIter)
        {
          if (ltable[index].confid < CONFLOOP)
            ltable[index].confid++;
          if (ltable[index].NbIter < 3)
          //just do not predict when the loop count is 1 or 2
          {
            // free the entry
            ltable[index].dir = Taken;
            ltable[index].NbIter = 0;
            ltable[index].age = 0;
            ltable[index].confid = 0;
          }
        }
        else
        {
          if (ltable[index].NbIter == 0)
          {
            // first complete nest;
            ltable[index].confid = 0;
            ltable[index].NbIter = ltable[index].CurrentIter;
          }
          else
          {
            //not the same number of iterations as last time: free the entry
            ltable[index].NbIter = 0;
            ltable[index].confid = 0;
          }
        }
        ltable[index].CurrentIter = 0;
      }
    }
    else if (ALLOC)

    {
      uint32_t X = MYRANDOM() & 3;

      if ((MYRANDOM() & 3) == 0)
        for (int i = 0; i < 4; i++)
        {
          int LHIT = (X + i) & 3;
          int index = (LI ^ ((LIB >> LHIT) << 2)) + LHIT;
          if (ltable[index].age == 0)
          {
            ltable[index].dir = !Taken;
            // most of mispredictions are on last iterations
            ltable[index].TAG = LTAG;
            ltable[index].NbIter = 0;
            ltable[index].age = 7;
            ltable[index].confid = 0;
            ltable[index].CurrentIter = 0;
            break;
          }
          else
            ltable[index].age--;
          break;
        }
    }
  }
#endif

  bool getbim()
  {
    BIM = (btable[BI].pred << 1) + (btable[BI >> HYSTSHIFT].hyst);
    HighConf = (BIM == 0) || (BIM == 3);
    return (btable[BI].pred > 0);
  }

  void baseupdate(bool Taken)
  {
    int inter = BIM;
    if (Taken)
    {
      if (inter < 3)
        inter += 1;
    }
    else if (inter > 0)
      inter--;
    btable[BI].pred = inter >> 1;
    btable[BI >> HYSTSHIFT].hyst = (inter & 1);
  };

  //just a simple pseudo random number generator: use available information
  // to allocate entries  in the loop predictor
  int MYRANDOM()
  {
    Seed++;
    Seed ^= phist;
    Seed = (Seed >> 21) + (Seed << 11);
    return (Seed);
  };

  //  TAGE PREDICTION: same code at fetch or retire time but the index and tags must recomputed
  void Tagepred(UINT32 PC)
  {
    HitBank = 0;
    AltBank = 0;
    for (int i = 1; i <= NHIST; i++)
    {
      GI[i] = gindex(PC, i, phist, ch_i);
      GTAG[i] = gtag(PC, i, ch_t[0], ch_t[1]);
    }

    BI = PC & ((1 << LOGB) - 1);

    //Look for the bank with longest matching history
    for (int i = NHIST; i > 0; i--)
    {
      if (gtable[i][GI[i]].tag == GTAG[i])
      {
        HitBank = i;
        LongestMatchPred = (gtable[HitBank][GI[HitBank]].ctr >= 0);
        break;
      }
    }

    //Look for the alternate bank
    for (int i = HitBank - 1; i > 0; i--)
    {
      if (gtable[i][GI[i]].tag == GTAG[i])
      {

        AltBank = i;
        break;
      }
    }
    //computes the prediction and the alternate prediction

    if (HitBank > 0)
    {
      if (AltBank > 0)
        alttaken = (gtable[AltBank][GI[AltBank]].ctr >= 0);
      else
        alttaken = getbim();

      //if the entry is recognized as a newly allocated entry and
      //USE_ALT_ON_NA is positive  use the alternate prediction
      int index = INDUSEALT ^ LongestMatchPred;
      bool Huse_alt_on_na =
          (use_alt_on_na[index][HitBank > (NHIST / 3)] >= 0);

      if ((!Huse_alt_on_na) || (abs(2 * gtable[HitBank][GI[HitBank]].ctr + 1) > 1))
        tage_pred = LongestMatchPred;
      else
        tage_pred = alttaken;

      HighConf =
          (abs(2 * gtable[HitBank][GI[HitBank]].ctr + 1) >=
           (1 << CWIDTH) - 1);
      //	LowConf = (abs (2 * gtable[HitBank][GI[HitBank]].ctr + 1) == 1);
    }
    else
    {
      alttaken = getbim();
      tage_pred = alttaken;
      LongestMatchPred = alttaken;
    }
  }
  //compute the prediction

  bool GetPrediction(UINT32 PC)
  {
    // computes the TAGE table addresses and the partial tags

    Tagepred(PC);
    pred_taken = tage_pred;

#ifdef LOOPPREDICTOR
    predloop = getloop(PC); // loop prediction
    pred_taken = ((WITHLOOP >= 0) && (LVALID)) ? predloop : pred_taken;
#endif

    pred_inter = pred_taken;
    if (!(HighConf))
    {
      CheckStatistic(PC, pred_taken);

      if (IsStatistic)
        pred_taken = pred_Statistic;
    }
    return (pred_taken);
  }

  //  UPDATE  FETCH HISTORIES   + spec update of the loop predictor + Update of the IUM
  void FetchHistoryUpdate(uint32_t PC, int16_t brtype, bool taken,
                          uint32_t target)
  {

    if (brtype == OPTYPE_BRANCH_COND)
    {

      HistoryUpdate(PC, brtype, taken, target, phist,
                    ptghist, ch_i, ch_t[0],
                    ch_t[1]);
    }
  }

  void HistoryUpdate(uint32_t PC, uint16_t brtype, bool taken,
                     uint32_t target, long long &X, int &Y,
                     folded_history *H, folded_history *G,
                     folded_history *J)
  {
    //special treatment for unconditional branchs: 4 bits
    int maxt;
    if (brtype == OPTYPE_BRANCH_COND)
      maxt = 1;
    else
      maxt = 4;

    int T = ((PC) << 1) + taken;
    int PATH = PC ^ (PC >> 2);

    for (int t = 0; t < maxt; t++)
    {
      bool DIR = (T & 1);
      T >>= 1;
      int PATHBIT = (PATH & 127);
      PATH >>= 1;
      //update  history
      Y--;
      ghist[Y & (HISTBUFFERLENGTH - 1)] = DIR;
      X = (X << 1) ^ PATHBIT;
      for (int i = 1; i <= NHIST; i++)
      {

        H[i].update(ghist, Y);
        G[i].update(ghist, Y);
        J[i].update(ghist, Y);
      }
    }

    //END UPDATE  HISTORIES
  }

  // PREDICTOR UPDATE

  void UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir,
                       UINT32 branchTarget)
  {

#ifdef LOOPPREDICTOR
    if (LVALID)
    {

      if (pred_taken != predloop)
        ctrupdate(WITHLOOP, (predloop == resolveDir), 7);
    }

    loopupdate(PC, resolveDir, (pred_taken != resolveDir));

#endif

    UpdateStatistic(PC, pred_inter, resolveDir);
    //TAGE UPDATE
    bool ALLOC = ((tage_pred != resolveDir) & (HitBank < NHIST));
    if ((IsStatistic) & (pred_Statistic == resolveDir))
      ALLOC = false;

    if (pred_taken == resolveDir)
      if ((MYRANDOM() & 31) != 0)
        ALLOC = false;
    //do not allocate too often if the overall prediction is correct

    if (HitBank > 0)
    {
      // Manage the selection between longest matching and alternate matching
      // for "pseudo"-newly allocated longest matching entry
      bool PseudoNewAlloc =
          (abs(2 * gtable[HitBank][GI[HitBank]].ctr + 1) <= 1);
      // an entry is considered as newly allocated if its prediction counter is weak
      if (PseudoNewAlloc)
      {
        if (LongestMatchPred == resolveDir)
          ALLOC = false;
        // if it was delivering the correct prediction, no need to allocate a new entry
        //even if the overall prediction was false
        if (LongestMatchPred != alttaken)
        {
          int index = (INDUSEALT) ^ LongestMatchPred;
          ctrupdate(use_alt_on_na[index]
                                 [HitBank > (NHIST / 3)],
                    (alttaken == resolveDir), 4);
        }
      }
    }

    if (tage_pred == resolveDir)
    {
      if ((MYRANDOM() & 15) == 0)
        ctrupdate(AllocCount, true, 3);
    }
    else
    {
      ctrupdate(AllocCount, false, 3);
    }
    //AllocCount becomes negative when burst of mispredictions
    if (ALLOC)
    {

      int T = NNN * ((AllocCount >= 0));

      int A = 1;
      if ((MYRANDOM() & 127) < 32)
        A = 2;
      int Penalty = 0;
      int NA = 0;
      for (int i = HitBank + A; i <= NHIST; i += 1)
      {
        if (gtable[i][GI[i]].u == 0)
        {
          gtable[i][GI[i]].tag = GTAG[i];
          gtable[i][GI[i]].ctr = (resolveDir) ? 0 : -1;
          NA++;
          if (T <= 0)
          {
            break;
          }
          i += 1;
          T -= 1;
        }
        else
        {
          bool DEC = false;
          DEC = (TICK >= (MYRANDOM() & 127));
          if (DEC)
            if (T == NNN)
              if ((MYRANDOM() & 1))
                gtable[i][GI[i]].u--;
          Penalty++;
        }
      }
      TICK += (Penalty - NA);
      if (TICK < 0)
        TICK = 0;
      if (TICK > 1023)
      {
        TICK = 1023;
      }
    }

    if (HitBank > 0)
    {
      if (abs(2 * gtable[HitBank][GI[HitBank]].ctr + 1) == 1)
        if (LongestMatchPred != resolveDir)

        { // acts as a protection
          if (AltBank > 0)
          {
            ctrupdate(gtable[AltBank][GI[AltBank]].ctr,
                      resolveDir, CWIDTH);
          }
          if (AltBank == 0)
            baseupdate(resolveDir);
        }
      ctrupdate(gtable[HitBank][GI[HitBank]].ctr, resolveDir, CWIDTH);
      //if sign changes: no way it can have been useful
      if (abs(2 * gtable[HitBank][GI[HitBank]].ctr + 1) == 1)
        gtable[HitBank][GI[HitBank]].u = 0;
    }
    else
      baseupdate(resolveDir);

    if (LongestMatchPred != alttaken)
    {
      if (LongestMatchPred == resolveDir)
      {
        if (gtable[HitBank][GI[HitBank]].u < (1 << UWIDTH) - 1)
          gtable[HitBank][GI[HitBank]].u++;
      }
      else if (gtable[HitBank][GI[HitBank]].u > 0)
        gtable[HitBank][GI[HitBank]].u--;
    }

    FetchHistoryUpdate(PC, OPTYPE_BRANCH_COND, resolveDir, branchTarget);

    //END PREDICTOR UPDATE
  }
  void CheckStatistic(uint32_t pc, bool pred)
  {
    IsStatistic = false;
    HitStatistic = false;
    for (int i = 0; i < 2; i++)
    {
      int inter =
          (pcstat ^ (((i)&1) * (pcstat >> (LOGSTATISTIC - 2)))) &
          ((1 << (LOGSTATISTIC - 2)) - 1);
      indexStatistic = 2 * ((inter << 1) + pred) + ((i)&1);

      if (Statistic[indexStatistic].tag == pctag)
      {
        if (abs(2 * Statistic[indexStatistic].ctr + 1) >= 9)
        {
          IsStatistic = true;
        }
        HitStatistic = true;

        pred_Statistic = (Statistic[indexStatistic].ctr >= 0);

        return;
      }
    }
  }

  void UpdateStatistic(uint32_t pc, bool pred, bool taken)
  {

    if (HighConf)
      return;
    if (pred == taken)
    {
      if (!HitStatistic)
        return;
    }
    if (HitStatistic)
    {

      ctrupdate(Statistic[indexStatistic].ctr, taken, SFWIDTH);
      return;
    }

    if (MYRANDOM() & 15)
      return;
    int X = MYRANDOM() & 1;
    for (int i = 0; i < 2; i++)
    {
      int inter =
          (pcstat ^ (((i + X) & 1) * (pcstat >> (LOGSTATISTIC - 2)))) &
          ((1 << (LOGSTATISTIC - 2)) - 1);
      indexStatistic = 2 * ((inter << 1) + pred) + ((i + X) & 1);

      if ((abs(2 * Statistic[indexStatistic].ctr + 1) == 1) || ((Statistic[indexStatistic].ctr >= 0) == pred))
      {
        Statistic[indexStatistic].tag = pctag;
        Statistic[indexStatistic].ctr = (taken) ? 0 : -1;
        return;
      }
    }

    if ((MYRANDOM() & 7) == 0)
      for (int i = 0; i < 2; i++)
      {
        int inter =
            (pcstat ^ (((i + X) & 1) * (pcstat >> (LOGSTATISTIC - 2)))) &
            ((1 << (LOGSTATISTIC - 2)) - 1);
        indexStatistic = 2 * ((inter << 1) + pred) + ((i + X) & 1);

        if (!pred)
          Statistic[indexStatistic].ctr--;
        if (pred)
          Statistic[indexStatistic].ctr++;

        return;
      }
  }

  void TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget)
  {

    bool taken = true;
    switch (opType)
    {
    case OPTYPE_CALL_DIRECT:
    case OPTYPE_INDIRECT_BR_CALL:
    case OPTYPE_RET:
    case OPTYPE_BRANCH_UNCOND:
      HistoryUpdate(PC, opType, taken, branchTarget, phist,
                    ptghist, ch_i,
                    ch_t[0], ch_t[1]);
      break;

    default:;
    }
  }
};

#endif
