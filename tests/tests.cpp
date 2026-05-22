#include "Account.h"
#include "Transaction.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class AccountMock : public Account {
public:
    AccountMock(int id, int balance) : Account(id, balance) {}
    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int diff));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};
class TransactionMock : public Transaction {
public:
    MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
};

TEST(Account, Mock) {
    AccountMock acc_mock(1, 100);
    EXPECT_CALL(acc_mock, GetBalance()).Times(1);
    EXPECT_CALL(acc_mock, ChangeBalance(testing::_)).Times(2);
    EXPECT_CALL(acc_mock, Lock()).Times(2);
    EXPECT_CALL(acc_mock, Unlock()).Times(1);
    acc_mock.GetBalance();
    acc_mock.ChangeBalance(100); // throw
    acc_mock.Lock();
    acc_mock.ChangeBalance(100);
    acc_mock.Lock(); // throw
    acc_mock.Unlock();
}

TEST(Account, SimpleTest) {
    Account acc(1, 100);
    EXPECT_EQ(acc.id(), 1);
    EXPECT_EQ(acc.GetBalance(), 100);
    EXPECT_THROW(acc.ChangeBalance(100), std::runtime_error);
    EXPECT_NO_THROW(acc.Lock());
    acc.ChangeBalance(100);
    EXPECT_EQ(acc.GetBalance(), 200);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(Transaction, Mock) {
    TransactionMock tr_mock;
    Account acc1(1, 50);
    Account acc2(2, 500);
    EXPECT_CALL(tr_mock, Make(testing::_, testing::_, testing::_))
    .Times(6);
    tr_mock.set_fee(100);
    tr_mock.Make(acc1, acc2, 199);
    tr_mock.Make(acc2, acc1, 500);
    tr_mock.Make(acc2, acc1, 300);
    tr_mock.Make(acc1, acc1, 0); // throw
    tr_mock.Make(acc1, acc2, -1); // throw
    tr_mock.Make(acc1, acc2, 99); // throw
}

TEST(Transaction, SimpleTest) {
    Transaction tr;
    Account acc1(1, 50);
    Account acc2(2, 500);
    tr.set_fee(100);
    EXPECT_EQ(tr.fee(), 100);
    EXPECT_THROW(tr.Make(acc1, acc1, 0), std::logic_error);
    EXPECT_THROW(tr.Make(acc1, acc2, -1), std::invalid_argument);
    EXPECT_THROW(tr.Make(acc1, acc2, 99), std::logic_error);
    EXPECT_FALSE(tr.Make(acc1, acc2, 199));
    EXPECT_FALSE(tr.Make(acc2, acc1, 500));
    EXPECT_FALSE(tr.Make(acc2, acc1, 300));
}
