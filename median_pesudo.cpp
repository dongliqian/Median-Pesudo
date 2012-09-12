// Master:
// 默认每次猜100个数
static int GUESS_NUM = 100;
static int64_t TOTAL_NUM;

int main()
{
  int start = std::numeric_limits<int64_t>::min();
  int end = std::numeric_limits<int64_t>::max();

  TOTAL_NUM = total_reduce();

  while(start != end)
  {
    median_reduce(start, end, &start, &end);
  }

  cout << start;

  return 0;
}

int64_t total_reduce()
{
  int total_number = 0;

  FOR_EACH_WORKER
  {
    total_number += worker.query_total();
  }

  return total_number;
}

void median_reduce(int64_t start, int64_t end, int64_t * newStart, int64_t * newEnd)
{
  vector<int64_t> guesses(GUESS_NUM);
  vector< pair<int64_t, int64_t> > ranks(GUESS_NUM);

  for (int i = 0; i < GUESS_NUM; i++)
  {
    int64_t guess = start + (end - start) * i / GUESS_NUM;
    guesses[i] = guess;
  }

  FOR_EACH_WORKER
  {
    vector< pair<int64_t, int64_t> > ranks_one_worker(GUESS_NUM);
    worker.query_ranks(guesses, ranks_one_worker);

    for (int i = 0; i < GUESS_NUM; i++)
    {
      ranks[i].first += ranks_one_worker[i].first;
      ranks[i].second += ranks_one_worker[i].second;
    }
  }

  for (int i = 0; i < GUESS_NUM; i++)
  {
    if (ranks[i].first >= TOTAL_NUM / 2)
    {
      *newStart = guesses[i];
      break;
    }
  }

  for (int i = GUESS_NUM; i >= 0; i--)
  {
    if (ranks[i].second <= TOTAL_NUM / 2)
    {
      *newEnd = guesses[i];
      break;
    }
  }
}


// Worker:

int query_total()
{
  return _data.size();
}

void query_ranks(vector<int64_t> & guesses, vector< pair<int64_t, int64_t> > & ranks)
{
  for (int i = 0; i < guesses.size(); i++)
  {
    int64_t guess = guesses[i];
    pair<int64_t, int64_t> rank = calc_rand(guess);//计算guess的开始排名和结束排名
    ranks.push_back(rank);
  }
}
