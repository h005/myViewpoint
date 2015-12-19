#-*-coding:utf-8-*-

class Question:
    def __init__(self, item):
        self.item = item
        self.url = 'http://dn-goodview.qbox.me/' + item

    def resp(self):
        template = \
u'''
  <Question> 
    <QuestionIdentifier>%s</QuestionIdentifier>  
    <QuestionContent> 
      <Text>%s</Text>  
      <Binary> 
        <MimeType> 
          <Type>image</Type>  
          <SubType>jpg</SubType> 
        </MimeType>  
        <DataURL>%s</DataURL>  
        <AltText>aaa</AltText> 
      </Binary> 
    </QuestionContent>  
    <AnswerSpecification> 
      <SelectionAnswer> 
        <StyleSuggestion>radiobutton</StyleSuggestion>  
        <Selections> 
          <Selection> 
            <SelectionIdentifier>good</SelectionIdentifier>  
            <Text>I think the viewpoint is good</Text> 
          </Selection>  
          <Selection> 
            <SelectionIdentifier>bad</SelectionIdentifier>  
            <Text>I think the viewpoint is bad</Text> 
          </Selection>
        </Selections> 
      </SelectionAnswer> 
    </AnswerSpecification> 
  </Question>
'''
        return template % (self.item, self.item, self.url)


class Survey:
    def __init__(self, identity, items):
        self.identity = identity
        self.questions = [Question(item) for item in items]

    def question_resp(self):
        template = \
u'''<?xml version="1.0" encoding="utf-8"?>
<QuestionForm xmlns="http://mechanicalturk.amazonaws.com/AWSMechanicalTurkDataSchemas/2005-10-01/QuestionForm.xsd">
  <Overview>
    <Title>Evaluate these photos' viewpoints</Title>
    <Text>Feel free to give us your opinions about these photos, focus on its' viewpoints</Text>
  </Overview>
  
  %s
</QuestionForm>
'''
        strs = [q.resp() for q in self.questions]
        return template % '\n'.join(strs)

    def property_resp(self):
        template = \
u'''title:%s
description:We need your opinions whether the viewpoints of photos are good or bad.
keywords:viewpoint, evaluation 
reward:0.01
assignments:20
annotation:sample#command

######################################
## HIT Timing Properties
######################################

# this Assignment Duration value is 60 * 60 = 1 hour
assignmentduration:3600

# this HIT Lifetime value is 60*60*24*3 = 3 days
hitlifetime:259200

# this Auto Approval period is 60*60*24*15 = 15 days
autoapprovaldelay:1296000'''
        return template % self.identity

if __name__ == '__main__':
    a = Survey('goodview10', ['kxm/img0000.jpg', 'kxm/img0001.jpg'])
    with open('moviesurvey.properties', 'w') as f:
        f.write(a.property_resp().encode('utf-8'))

    with open('moviesurvey.question', 'w') as f:
        f.write(a.question_resp().encode('utf-8'))
